pload ALL

# Base path for input STEP files
set input_path "c:\\NIST-PMI-STEP-Files\\"

# Create a list of STEP filenames
set file_names {
    "nist_stc_10_asme1_ap242-e2.stp"
    "nist_ctc_01_asme1_ap242-e1.stp"
    "nist_ctc_02_asme1_ap242-e2.stp"
    "nist_ctc_03_asme1_ap242-e2.stp"
    "nist_ctc_04_asme1_ap242-e1.stp"
    "nist_ctc_05_asme1_ap242-e1.stp"
    "nist_ftc_06_asme1_ap242-e2.stp"
    "nist_ftc_07_asme1_ap242-e2.stp"
    "nist_ftc_08_asme1_ap242-e1-tg.stp"
    "nist_ftc_08_asme1_ap242-e2.stp"
    "nist_ftc_09_asme1_ap242-e1.stp"
    "nist_ftc_10_asme1_ap242-e2.stp"
    "nist_ftc_11_asme1_ap242-e2.stp"
    "nist_stc_06_asme1_ap242-e3.stp"
    "nist_stc_07_asme1_ap242-e3.stp"
    "nist_stc_08_asme1_ap242-e3.stp"
    "nist_stc_09_asme1_ap242-e3.stp"
}

set output_folder "c:/Test_PMI"

# Create output directory if it doesn't exist
if {![file exists $output_folder]} {
    puts "Creating output directory: $output_folder"
    file mkdir $output_folder
}

# Configure triangulation settings
StoreTriangulation 1

vinit

# Iterate through each file name
foreach file_name $file_names {
    # Construct full input path
    set full_input_path "${input_path}${file_name}"
    puts "Processing file: $full_input_path"
    
    # Check if file exists before processing
    if {[file exists $full_input_path]} {
        # Read the STEP file
        ReadFile D $full_input_path
        
        # Display with auto-triangulation and shaded mode
        XDisplay D -autoTriang 1 -dispMode 1
        
        # Extract filename without extension for output
        set file_base [file rootname $file_name]
        set output_path "${output_folder}/${file_base}.xbf"
        
        # Save as XBF format
        puts "Saving to: $output_path"
        XSave D $output_path
        
        # Close the document
        Close D
        
        puts "Completed processing: $file_name"
    } else {
        puts "Warning: File not found: $full_input_path"
    }
}

puts "All files processed."