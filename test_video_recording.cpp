#include <iostream>
#include <Image_VideoRecorder.hxx>
#include <Image_PixMap.hxx>
#include <Message.hxx>

int main()
{
    std::cout << "Testing OpenCascade Video Recording..." << std::endl;
    
    // Create video recorder
    Handle(Image_VideoRecorder) recorder = new Image_VideoRecorder();
      // Set up video parameters
    Image_VideoParams params;
    params.Width = 640;
    params.Height = 480;
    params.SetFramerate(30);  // 30 FPS
    params.Format = "avi";        // Try AVI format
    params.VideoCodec = "mpeg4";  // Try mpeg4 codec instead of libx264
    params.PixelFormat = "yuv420p";
      // Try to open video file
    const char* filename = "test_output.avi";
    std::cout << "Opening video file: " << filename << std::endl;
    
    if (!recorder->Open(filename, params))
    {
        std::cout << "ERROR: Failed to open video file for recording!" << std::endl;
        return 1;
    }
    
    std::cout << "Video file opened successfully!" << std::endl;
    
    // Create a simple test frame (red gradient)
    Image_PixMap& frame = recorder->ChangeFrame();
    if (!frame.InitTrash(Image_Format_RGBA, params.Width, params.Height))
    {
        std::cout << "ERROR: Failed to initialize frame buffer!" << std::endl;
        recorder->Close();
        return 1;
    }
    
    // Fill with a simple pattern for testing
    for (int y = 0; y < params.Height; ++y)
    {
        for (int x = 0; x < params.Width; ++x)
        {
            Standard_Byte* pixel = frame.ChangeData() + (y * frame.SizeRowBytes()) + (x * 4);
            pixel[0] = (Standard_Byte)(255 * x / params.Width);  // Red gradient
            pixel[1] = (Standard_Byte)(255 * y / params.Height); // Green gradient
            pixel[2] = 0;   // Blue
            pixel[3] = 255; // Alpha
        }
    }
    
    // Write a few test frames
    std::cout << "Writing test frames..." << std::endl;
    for (int i = 0; i < 90; ++i)  // 3 seconds at 30 FPS
    {
        // Modify the pattern slightly for each frame
        for (int y = 0; y < params.Height; y += 10)
        {
            for (int x = 0; x < params.Width; x += 10)
            {
                Standard_Byte* pixel = frame.ChangeData() + (y * frame.SizeRowBytes()) + (x * 4);
                pixel[2] = (Standard_Byte)(255 * i / 90);  // Blue increases over time
            }
        }
        
        if (!recorder->PushFrame())
        {
            std::cout << "ERROR: Failed to write frame " << i << std::endl;
            recorder->Close();
            return 1;
        }
        
        if ((i + 1) % 30 == 0)
        {
            std::cout << "Wrote " << (i + 1) << " frames (" << ((i + 1) / 30) << " seconds)" << std::endl;
        }
    }
    
    std::cout << "Closing video file..." << std::endl;
    recorder->Close();
    
    std::cout << "Video recording test completed successfully!" << std::endl;
    std::cout << "Output file: " << filename << std::endl;
    
    return 0;
}
