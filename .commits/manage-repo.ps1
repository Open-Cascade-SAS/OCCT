# OCCT native fork repository operations.
#
# Contract (same as the other owned repositories; see this repository's AGENTS.md):
#   - OCCT_INTERNAL_VERSION holds the fork's internal version <breaking>.<major>.<minor>.<build>.
#     Build acceptance owns it. This script reads it and never changes it.
#     Any -Version must equal it.
#   - Mutating actions (branch, add, commit, push, tag) need an attached branch and -Yes.
#     commit needs everything staged (no unstaged or untracked paths).
#     push and tag need a clean tree. There are no interactive prompts.
#   - push targets only paulnamalomba/OCCT. Open-Cascade-SAS/OCCT is a read/fetch reference and is never a target.
#   - tag creates an annotated tag native/v<internal version> and pushes only that tag.
#     It needs .commits/builds/<version>.json (accepted, status passed, source_commit) and a
#     family note that records the version. The unbuilt seed therefore cannot be tagged.
#     Existing tags (local or remote) are never moved or replaced.
#   - There is no GitHub Release action here. The family action of the frontend script is disabled.
#   - -WhatIf (alias -DryRun) runs every check and prints the mutating commands without
#     running them. Read-only network calls (git ls-remote) still run.
#   - -Action check runs self-checks and the repository consistency checks. It writes nothing
#     to the repository and uses no network.
#
# Usage:
#   pwsh -NoProfile -File .commits/manage-repo.ps1 -Action list
#   pwsh -NoProfile -File .commits/manage-repo.ps1 -Action check
#   pwsh -NoProfile -File .commits/manage-repo.ps1 -Action commit -Version 8.0.1.1 -Message "summary" -Yes
#   pwsh -NoProfile -File .commits/manage-repo.ps1 -Action push -Version 8.0.1.1 -Yes
#   pwsh -NoProfile -File .commits/manage-repo.ps1 -Action tag -Version 8.0.1.1 -Yes -WhatIf
param(
    [ValidateSet('list', 'status', 'branch', 'add', 'commit', 'push', 'tag', 'family', 'check', 'help')]
    [string]$Action = 'list',
    [string]$Version = '',
    [string]$Branch = '',
    [string]$Message = '',
    [string]$Remote = 'origin',
    [string]$NotesFile = '',
    [string]$Trailer = '',
    [string[]]$Assets = @(),
    [switch]$Force,
    [switch]$Yes,
    [Alias('DryRun')]
    [switch]$WhatIf
)
$ErrorActionPreference = 'Stop'

# ---- Repository configuration (the only part that differs between owned repositories) ----
$script:Product = 'OCCT native fork'
$script:VersionFile = 'OCCT_INTERNAL_VERSION'
$script:ExpectedRepository = 'github.com/paulnamalomba/OCCT'
$script:BuildTagPrefix = 'native/v'
$script:VersionPattern = '^(0|[1-9]\d*)\.([0-9])\.([0-9])\.([0-9])$'
$script:MultiDigitFirstField = $true
$script:RecordKind = 'fork'
$script:AllowFamilyRelease = $false
$script:FamilyTagPrefix = 'v'
# ------------------------------------------------------------------------------------------

$script:RepoRoot = Split-Path $PSScriptRoot -Parent
$script:GitExe = (Get-Command git -ErrorAction Stop).Source
$env:GIT_TERMINAL_PROMPT = '0'

function Invoke-Git {
    param([string[]]$Arguments, [switch]$AllowFailure, [string]$Root = $script:RepoRoot)
    $output = & $script:GitExe -C $Root @Arguments 2>&1
    $script:GitExit = $LASTEXITCODE
    if ($script:GitExit -ne 0 -and -not $AllowFailure) { throw ($output | Out-String) }
    return $output
}

function Get-GitText {
    param([string[]]$Arguments, [string]$Root = $script:RepoRoot)
    return (($(Invoke-Git -Arguments $Arguments -Root $Root) | Out-String).Trim())
}

function Test-VersionText {
    param([string]$Text)
    return ($Text -cmatch $script:VersionPattern)
}

function Get-FamilyOf {
    param([string]$Text)
    return (($Text -split '\.')[0..2] -join '.')
}

function ConvertTo-NormalizedRemote {
    param([string]$Url)
    $text = $Url.Trim()
    $text = $text -replace '^https://', '' -replace '^ssh://git@', '' -replace '^git@([^:/]+):', '$1/' -replace '\.git/?$', ''
    return $text.TrimEnd('/').ToLowerInvariant()
}

function Assert-RemoteName {
    if ($Remote -notmatch '^[A-Za-z0-9][A-Za-z0-9._-]*$') { throw "Invalid remote name: $Remote" }
}

function Assert-Remote {
    Assert-RemoteName
    $urls = @(Invoke-Git -Arguments @('remote', 'get-url', '--push', '--all', $Remote))
    if ($urls.Count -ne 1) { throw 'Exactly one owned push URL is required' }
    $normalized = ConvertTo-NormalizedRemote ($urls[0].ToString())
    if ($normalized -ne $script:ExpectedRepository) {
        throw "Remote $Remote does not push to $script:ExpectedRepository"
    }
}

function Get-CurrentBranch {
    param([string]$Root = $script:RepoRoot)
    $name = Invoke-Git -Arguments @('symbolic-ref', '--quiet', '--short', 'HEAD') -AllowFailure -Root $Root
    if ($script:GitExit -ne 0) { return $null }
    return (($name | Out-String).Trim())
}

function Assert-Branch {
    $current = Get-CurrentBranch
    if (-not $current) {
        throw 'Detached HEAD: create a branch with -Action branch -Branch <name> before changing or publishing the repository'
    }
    if ($Branch -and $Branch -ne $current) { throw "Current branch $current differs from -Branch $Branch" }
    return $current
}

function Test-CleanTree {
    param([string]$Root = $script:RepoRoot)
    return (-not (Get-GitText -Arguments @('status', '--porcelain') -Root $Root))
}

function Assert-CleanTree {
    if (-not (Test-CleanTree)) { throw 'Working tree is not clean (tracked changes or untracked files); commit or remove them first' }
}

function Test-IsAncestor {
    param([string]$Ancestor, [string]$Descendant, [string]$Root = $script:RepoRoot)
    $null = Invoke-Git -Arguments @('merge-base', '--is-ancestor', $Ancestor, $Descendant) -AllowFailure -Root $Root
    return ($script:GitExit -eq 0)
}

function Assert-Confirmed {
    param([string]$Description)
    if ($WhatIf) { return }
    if (-not $Yes) { throw "$Description requires -Yes (or -WhatIf to preview)" }
}

function Invoke-Mutation {
    param([string]$Exe, [string[]]$Arguments, [string]$Label)
    if ($WhatIf) {
        Write-Output "WHATIF: $Label $($Arguments -join ' ')"
        return
    }
    $output = & $Exe @Arguments 2>&1
    if ($LASTEXITCODE -ne 0) { throw ($output | Out-String) }
    $output | Write-Output
}

function Invoke-GitMutation {
    param([string[]]$Arguments)
    Invoke-Mutation -Exe $script:GitExe -Arguments (@('-C', $script:RepoRoot) + $Arguments) -Label 'git'
}

function Read-VersionFile {
    param([string]$Root = $script:RepoRoot)
    $path = Join-Path $Root $script:VersionFile
    if (-not (Test-Path -LiteralPath $path)) { throw "Missing version source $script:VersionFile" }
    return ((Get-Content -LiteralPath $path -Raw).Trim([char]0xFEFF, ' ', "`r", "`n", "`t"))
}

function Get-FamilyNotePath {
    param([string]$Family, [string]$Commits = $PSScriptRoot)
    $md = Join-Path $Commits "$Family.md"
    if (Test-Path -LiteralPath $md) { return $md }
    $txt = Join-Path $Commits "$Family.txt"
    if (Test-Path -LiteralPath $txt) { return $txt }
    return $null
}

# Returns a list of problems; empty means the accepted build record and family note are consistent.
function Test-BuildEvidence {
    param([string]$Text, [string]$Commits = $PSScriptRoot, [string]$Root = $script:RepoRoot)
    $problems = New-Object System.Collections.Generic.List[string]
    $family = Get-FamilyOf $Text
    $note = Get-FamilyNotePath -Family $family -Commits $Commits
    if (-not $note) {
        $problems.Add("Missing family note .commits/$family.md")
    } elseif ((Get-Content -LiteralPath $note -Raw) -notmatch [regex]::Escape($Text)) {
        $problems.Add("Family note $(Split-Path $note -Leaf) does not record $Text")
    }
    $recordPath = Join-Path $Commits "builds/$Text.json"
    if (-not (Test-Path -LiteralPath $recordPath)) {
        $problems.Add("Missing accepted build record .commits/builds/$Text.json")
        return $problems
    }
    try { $record = Get-Content -LiteralPath $recordPath -Raw | ConvertFrom-Json }
    catch { $problems.Add("Build record is not valid JSON: $($_.Exception.Message)"); return $problems }
    $sourceCommit = $null
    if ($script:RecordKind -eq 'frontend') {
        if ($record.internal_version -ne $Text) { $problems.Add('Build record internal_version differs from the version') }
        if (-not (Test-VersionText ([string]$record.previous_version))) { $problems.Add('Build record previous_version is missing or malformed') }
        if (-not $record.checks -or @($record.checks).Count -eq 0) { $problems.Add('Build record has no checks') }
        if (-not $record.artifact) { $problems.Add('Build record names no artifact') }
        if ($record.source) { $sourceCommit = [string]$record.source.commit }
    } else {
        if ($record.accepted -ne $Text) { $problems.Add('Build record accepted differs from the version') }
        if ($record.status -ne 'passed') { $problems.Add('Build record status is not passed') }
        $sourceCommit = [string]$record.source_commit
    }
    if ($sourceCommit -cnotmatch '^[0-9a-f]{40}$') {
        $problems.Add('Build record source commit is not a full 40-character SHA')
    } else {
        $null = Invoke-Git -Arguments @('cat-file', '-e', "$sourceCommit^{commit}") -AllowFailure -Root $Root
        if ($script:GitExit -ne 0) { $problems.Add("Build record source commit $sourceCommit is not in this repository") }
        elseif (-not (Test-IsAncestor -Ancestor $sourceCommit -Descendant 'HEAD' -Root $Root)) {
            $problems.Add("Build record source commit $sourceCommit is not an ancestor of HEAD")
        }
    }
    return $problems
}

function Assert-PublishedHead {
    param([string]$BranchName)
    $ref = "refs/remotes/$Remote/$BranchName"
    $null = Invoke-Git -Arguments @('show-ref', '--verify', '--quiet', $ref) -AllowFailure
    if ($script:GitExit -ne 0) { throw "No remote-tracking ref $Remote/$BranchName; push the branch first" }
    if (-not (Test-IsAncestor -Ancestor 'HEAD' -Descendant $ref)) {
        throw "HEAD is not on $Remote/$BranchName; push the acceptance commit before tagging"
    }
}

function Get-TagState {
    param([string]$Tag)
    $local = Get-GitText -Arguments @('tag', '--list', $Tag)
    $remoteLines = @(Invoke-Git -Arguments @('ls-remote', '--tags', $Remote, "refs/tags/$Tag", "refs/tags/$Tag^{}"))
    $localTarget = ''
    if ($local) { $localTarget = Get-GitText -Arguments @('rev-parse', "refs/tags/$Tag^{commit}") }
    return [pscustomobject]@{
        Local       = [bool]$local
        LocalTarget = $localTarget
        LocalType   = $(if ($local) { Get-GitText -Arguments @('cat-file', '-t', "refs/tags/$Tag") } else { '' })
        Remote      = ($remoteLines.Count -gt 0)
    }
}

function Publish-AnnotatedTag {
    param([string]$Tag, [string[]]$TagArguments)
    $state = Get-TagState -Tag $Tag
    if ($state.Remote) { throw "Tag $Tag already exists on $Remote; published tags are immutable" }
    $head = Get-GitText -Arguments @('rev-parse', 'HEAD')
    if ($state.Local) {
        if ($state.LocalType -ne 'tag' -or $state.LocalTarget -ne $head) {
            throw "Local tag $Tag exists and does not annotate HEAD; it is never replaced"
        }
        Write-Output "Local annotated tag $Tag already targets HEAD; publishing it"
    } else {
        Invoke-GitMutation -Arguments (@('tag', '-a', $Tag) + $TagArguments + @($head))
    }
    Invoke-GitMutation -Arguments @('push', $Remote, "refs/tags/${Tag}:refs/tags/$Tag")
}

function Assert-Publishable {
    Assert-Remote
    $current = Assert-Branch
    Assert-CleanTree
    $problems = @(Test-BuildEvidence -Text $script:AcceptedVersion)
    if ($problems.Count -gt 0) { throw ("Build evidence is not publishable:`n" + ($problems -join "`n")) }
    Assert-PublishedHead -BranchName $current
}

function Invoke-Commit {
    $current = Assert-Branch
    Assert-Confirmed 'commit'
    $staged = @(Invoke-Git -Arguments @('diff', '--cached', '--name-only'))
    if ($staged.Count -eq 0) { throw 'Nothing staged' }
    $unmerged = @(Invoke-Git -Arguments @('diff', '--name-only', '--diff-filter=U'))
    if ($unmerged.Count -gt 0) { throw "Unresolved merge conflicts:`n$($unmerged -join "`n")" }
    $leftover = @(Invoke-Git -Arguments @('status', '--porcelain') | Where-Object { $_ -match '^(\?\?|.[MADRCU])' })
    if ($leftover.Count -gt 0) { Write-Output "Not part of this commit (unstaged or untracked):`n$($leftover -join "`n")" }
    if (-not $Message) { throw '-Message is required' }
    if ($Message -match '[\r\n]') { throw 'Commit summary must be one line' }
    $arguments = @('commit', '-m', "$script:Product $script:AcceptedVersion`: $Message")
    if ($Trailer) {
        if ($Trailer -notmatch '^[A-Za-z][A-Za-z-]*: [^\r\n]+$') { throw '-Trailer must be one "Key: value" line' }
        $arguments += @('-m', $Trailer)
    }
    Invoke-GitMutation -Arguments $arguments
}

function Invoke-Push {
    Assert-Remote
    $current = Assert-Branch
    Assert-CleanTree
    Assert-Confirmed 'push'
    $arguments = @('push', '--set-upstream')
    if ($Force) { $arguments += '--force-with-lease' }
    Invoke-GitMutation -Arguments ($arguments + @($Remote, "refs/heads/${current}:refs/heads/$current"))
}

function Invoke-BuildTag {
    Assert-Publishable
    Assert-Confirmed 'tag'
    $tag = "$script:BuildTagPrefix$script:AcceptedVersion"
    $head = Get-GitText -Arguments @('rev-parse', 'HEAD')
    $recordPath = Join-Path $PSScriptRoot "builds/$script:AcceptedVersion.json"
    $hash = (Get-FileHash -LiteralPath $recordPath -Algorithm SHA256).Hash.ToLowerInvariant()
    Publish-AnnotatedTag -Tag $tag -TagArguments @(
        '-m', "$script:Product $script:AcceptedVersion",
        '-m', "Acceptance commit and tag target: $head`nBuild record .commits/builds/$script:AcceptedVersion.json SHA-256: $hash")
}

function Invoke-FamilyRelease {
    if (-not $script:AllowFamilyRelease) { throw 'This repository has no family release action' }
    if (-not $Version) { throw '-Version is required and must equal the accepted version' }
    Assert-Publishable
    Assert-Confirmed 'family'
    $family = Get-FamilyOf $script:AcceptedVersion
    $record = Get-Content -LiteralPath (Join-Path $PSScriptRoot "builds/$script:AcceptedVersion.json") -Raw | ConvertFrom-Json
    if ((Get-FamilyOf ([string]$record.previous_version)) -eq $family) {
        throw "$script:AcceptedVersion is not the first accepted build of family $family; a family tag is created once, at that build"
    }
    $tag = "$script:FamilyTagPrefix$family"
    $notes = $NotesFile
    if (-not $notes) { $notes = Get-FamilyNotePath -Family $family }
    if (-not $notes -or -not (Test-Path -LiteralPath $notes)) { throw 'Notes file not found' }
    $notes = (Resolve-Path -LiteralPath $notes).Path
    if ((Get-Content -LiteralPath $notes -Raw) -notmatch [regex]::Escape($script:AcceptedVersion)) {
        throw "Notes file does not mention $script:AcceptedVersion"
    }
    foreach ($asset in $Assets) {
        if (-not (Test-Path -LiteralPath $asset -PathType Leaf)) { throw "Asset not found: $asset" }
    }
    $gh = Get-Command gh -ErrorAction SilentlyContinue
    if (-not $gh -and -not $WhatIf) { throw 'GitHub CLI gh is required for the family release' }
    $repository = $script:ExpectedRepository.Substring('github.com/'.Length)
    $state = Get-TagState -Tag $tag
    if ($state.Remote) { throw "Family tag $tag already exists on $Remote; it is immutable" }
    Publish-AnnotatedTag -Tag $tag -TagArguments @('-F', $notes)
    $ghExe = $(if ($gh) { $gh.Source } else { 'gh' })
    $arguments = @('release', 'create', $tag, '--repo', $repository, '--verify-tag',
        '--title', "$script:Product $family family", '--notes-file', $notes) + @($Assets | ForEach-Object { (Resolve-Path -LiteralPath $_).Path })
    Invoke-Mutation -Exe $ghExe -Arguments $arguments -Label 'gh'
}

# ---- Self-checks (-Action check): no repository writes, no network ----
function Invoke-SelfCheck {
    $results = New-Object System.Collections.Generic.List[object]
    $add = {
        param([string]$Name, [bool]$Ok, [string]$Detail = '')
        $results.Add([pscustomobject]@{ Name = $Name; Ok = $Ok; Detail = $Detail })
    }
    $throws = {
        param([scriptblock]$Block)
        try { & $Block | Out-Null; return $false } catch { return $true }
    }

    # Version pattern
    foreach ($text in @('0.1.2.1', '0.0.0.0', '9.9.9.9')) { & $add "version accepts $text" (Test-VersionText $text) }
    foreach ($text in @('0.1.2.10', '0.10.2.1', '0.1.2', '0.1.2.1x', ' 0.1.2.1', '0.1.2.-1', '', '0.1.2.1.1')) {
        & $add "version rejects '$text'" (-not (Test-VersionText $text))
    }
    & $add 'first field width matches configuration' ((Test-VersionText '10.1.2.3') -eq $script:MultiDigitFirstField)
    & $add 'family of 0.1.2.1 is 0.1.2' ((Get-FamilyOf '0.1.2.1') -eq '0.1.2')

    # Remote normalisation and ownership
    $own = $script:ExpectedRepository
    foreach ($url in @("https://$own", "https://$own.git", "https://$own/", ('git@' + ([regex]'/').Replace($own, ':', 1) + '.git'), "ssh://git@$own.git", "https://$($own.ToUpperInvariant())")) {
        & $add "remote accepts $url" ((ConvertTo-NormalizedRemote $url) -eq $own)
    }
    foreach ($url in @("https://token@$own", "http://$own", 'https://github.com/Open-Cascade-SAS/OCCT', 'https://github.com/FreeCAD/FreeCAD.git', 'DISABLED-upstream-is-read-only', "https://$own-evil")) {
        & $add "remote rejects $url" ((ConvertTo-NormalizedRemote $url) -ne $own)
    }

    # Tag names
    $tag = "$script:BuildTagPrefix" + '0.1.2.1'
    & $add 'build tag is outside the family namespace' ($tag -ne ($script:FamilyTagPrefix + '0.1.2') -and $tag -match '^[A-Za-z0-9._/-]+$')

    # Confirmation guard
    $savedYes = $Yes; $savedWhatIf = $WhatIf
    $script:Yes = $false
    & $add 'mutation without -Yes is refused' (& $throws { Assert-Confirmed 'x' })
    $script:Yes = $true
    & $add 'mutation with -Yes is allowed' (-not (& $throws { Assert-Confirmed 'x' }))
    $script:Yes = $false
    $script:WhatIf = $true
    & $add '-WhatIf needs no -Yes' (-not (& $throws { Assert-Confirmed 'x' }))
    $script:Yes = [bool]$savedYes; $script:WhatIf = [bool]$savedWhatIf

    # Temporary repository: clean-tree, detached-HEAD and ancestry gates
    $temp = Join-Path ([IO.Path]::GetTempPath()) ("manage-repo-check-" + [Guid]::NewGuid().ToString('N'))
    try {
        $null = New-Item -ItemType Directory -Path $temp
        $identity = @('-c', 'user.name=check', '-c', 'user.email=check@example.invalid', '-c', 'commit.gpgsign=false')
        $null = Invoke-Git -Root $temp -Arguments @('init', '--quiet', '--initial-branch=main')
        Set-Content -LiteralPath (Join-Path $temp 'a.txt') -Value 'one'
        $null = Invoke-Git -Root $temp -Arguments @('add', '-A')
        $null = Invoke-Git -Root $temp -Arguments ($identity + @('commit', '--quiet', '-m', 'one'))
        $first = Get-GitText -Root $temp -Arguments @('rev-parse', 'HEAD')
        & $add 'temp repository starts clean' (Test-CleanTree -Root $temp)
        Set-Content -LiteralPath (Join-Path $temp 'b.txt') -Value 'untracked'
        & $add 'untracked file makes the tree dirty' (-not (Test-CleanTree -Root $temp))
        Remove-Item -LiteralPath (Join-Path $temp 'b.txt')
        Add-Content -LiteralPath (Join-Path $temp 'a.txt') -Value 'edit'
        & $add 'tracked edit makes the tree dirty' (-not (Test-CleanTree -Root $temp))
        $null = Invoke-Git -Root $temp -Arguments @('checkout', '--quiet', '--', 'a.txt')
        & $add 'attached branch is reported' ((Get-CurrentBranch -Root $temp) -eq 'main')
        Set-Content -LiteralPath (Join-Path $temp 'a.txt') -Value 'two'
        $null = Invoke-Git -Root $temp -Arguments @('add', '-A')
        $null = Invoke-Git -Root $temp -Arguments ($identity + @('commit', '--quiet', '-m', 'two'))
        $second = Get-GitText -Root $temp -Arguments @('rev-parse', 'HEAD')
        & $add 'ancestor test is true for history' (Test-IsAncestor -Ancestor $first -Descendant $second -Root $temp)
        & $add 'ancestor test is false in reverse' (-not (Test-IsAncestor -Ancestor $second -Descendant $first -Root $temp))
        $null = Invoke-Git -Root $temp -Arguments @('checkout', '--quiet', '--detach', $first)
        & $add 'detached HEAD is reported' ($null -eq (Get-CurrentBranch -Root $temp))
    } finally {
        if (Test-Path -LiteralPath $temp) {
            Get-ChildItem -LiteralPath $temp -Recurse -Force | ForEach-Object { if ($_.Attributes -band [IO.FileAttributes]::ReadOnly) { $_.Attributes = 'Normal' } }
            Remove-Item -LiteralPath $temp -Recurse -Force
        }
    }

    # Repository consistency (read-only)
    $versionText = ''
    try { $versionText = Read-VersionFile } catch { & $add 'version file readable' $false $_.Exception.Message }
    if ($versionText) {
        & $add "version file $script:VersionFile holds a valid version" (Test-VersionText $versionText) $versionText
        if (Test-VersionText $versionText) {
            $problems = @(Test-BuildEvidence -Text $versionText)
            if ($problems.Count -eq 0) { & $add "family note and build record for $versionText" $true }
            elseif ($script:RecordKind -eq 'fork' -and -not (Test-Path -LiteralPath (Join-Path $PSScriptRoot "builds/$versionText.json"))) {
                # An unbuilt seed has no accepted record by definition; only the family note is required to exist.
                $noteProblems = @($problems | Where-Object { $_ -like '*family note*' })
                & $add "family note for unbuilt seed $versionText" ($noteProblems.Count -eq 0) ($noteProblems -join '; ')
            } else {
                & $add "family note and build record for $versionText" $false ($problems -join '; ')
            }
        }
    }
    $remoteOk = $true; $remoteDetail = ''
    try { Assert-Remote } catch { $remoteOk = $false; $remoteDetail = $_.Exception.Message.Trim() }
    & $add "push remote $Remote targets $script:ExpectedRepository" $remoteOk $remoteDetail

    $failed = 0
    foreach ($result in $results) {
        $mark = $(if ($result.Ok) { 'PASS' } else { 'FAIL'; $failed++ })
        $line = "$mark  $($result.Name)"
        if ($result.Detail) { $line += "  [$($result.Detail)]" }
        Write-Output $line
    }
    Write-Output "checks: $($results.Count - $failed) passed, $failed failed"
    if ($failed -gt 0) { throw "$failed self-check(s) failed" }
}

try {
    if ($Action -eq 'help') {
        Write-Output 'manage-repo.ps1 -Action list|status|branch|add|commit|push|tag|family|check|help [-Version accepted] [-Branch name] [-Message summary] [-Remote origin] [-Trailer "Key: value"] [-NotesFile path] [-Assets path,...] [-Force] [-Yes] [-WhatIf]'
        exit 0
    }
    $null = Invoke-Git -Arguments @('rev-parse', '--show-toplevel')
    $script:AcceptedVersion = Read-VersionFile
    if (-not (Test-VersionText $script:AcceptedVersion)) {
        throw "Invalid version in $script:VersionFile : '$script:AcceptedVersion'"
    }
    if ($Version -and $Version -cne $script:AcceptedVersion) {
        throw "-Version must equal $script:VersionFile ($script:AcceptedVersion)"
    }
    $family = Get-FamilyOf $script:AcceptedVersion
    if ($Action -in @('add', 'commit', 'push', 'tag', 'family')) {
        if (-not $Version) { throw "-Version is required for -Action $Action and must equal $script:AcceptedVersion" }
        $null = Assert-Branch
    }
    switch ($Action) {
        'list' {
            Write-Output "$script:Product $script:AcceptedVersion"
            $note = Get-FamilyNotePath -Family $family
            Write-Output "Version source: $script:VersionFile; family note: $(if ($note) { ".commits/$(Split-Path $note -Leaf)" } else { 'missing' })"
            Invoke-Git -Arguments @('status', '--short') | Write-Output
        }
        'status' { Invoke-Git -Arguments @('status', '--short') | Write-Output }
        'branch' {
            if (-not $Branch) { throw '-Branch is required' }
            $null = Invoke-Git -Arguments @('check-ref-format', '--branch', $Branch)
            Assert-Confirmed 'branch'
            Invoke-GitMutation -Arguments @('switch', '-c', $Branch)
        }
        'add' {
            Invoke-Git -Arguments @('status', '--short') | Write-Output
            Assert-Confirmed 'add'
            Invoke-GitMutation -Arguments @('add', '-A')
        }
        'commit' { Invoke-Commit }
        'push' { Invoke-Push }
        'tag' { Invoke-BuildTag }
        'family' { Invoke-FamilyRelease }
        'check' { Invoke-SelfCheck }
    }
    exit 0
} catch {
    Write-Error -Message $_ -ErrorAction Continue
    exit 1
}
