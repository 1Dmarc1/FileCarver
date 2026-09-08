# C File Carver

A simple lightweight, multithreaded file carving tool written in C.

- Supports JPEG, PNG, PDF, RIFF (WAV, AVI, WEBP), and AIFF


## Usage
./carver -i <input_file> -o <output_dir> [-t threads]

### Options
-i, --input   : Path to input disk image or raw binary (Required)
-o, --output  : Output directory for recovered files (Required)
-t, --threads : Number of worker threads (Optional, default: 1)
