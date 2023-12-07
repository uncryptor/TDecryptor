# TDecryptor
An Automatic Teardown .tde File De- and Encryptor

![Screenshot](Screenshots/TDecryptor.jpg)
![Screenshot](Screenshots/Files2.jpg)
![Screenshot](Screenshots/Files1.jpg)
![Cursor](Screenshots/Cursor.jpg)

# How to Use

TDecryptor can decrypt **and** (as of version 1.1) **re-encrypt** Teardown .tde files.  
The mode is set using command line parameters: `TDecryptor.exe "path\to\Teardown\" [mode]`,  
where `[mode]` may be replaced with `-D` to **decrypt** or `-E` to **re-encrypt**.  
If the parameter is not set, it defaults to **decrypt**.

- Open a command line (CMD) and navigate to the directory where TDecryptor.exe is located (e.g. TDecryptor/Build/Release/)
- To DECRYPT: Type in: `TDecryptor.exe "path\to\Teardown\" -D` and hit enter (**Replace 'path\to\Teardown\' with your actual Teardown installation path!**).
- The program will now create a directory called "DECRYPTED" at the root of your Teardown installation  
  and proceed to find, decrypt and dump all encrypted .tde files into that directory.  
  It also keeps the directory structure of these files, allowing for easy browsing and finding specific files.
- To ENCRYPT: Type in: `TDecryptor.exe "path\to\Teardown\" -E` and hit enter (**Replace 'path\to\Teardown\' with your actual Teardown installation path!**).
- The program will then read all decrypted files from the 'DECRYPTED' directory, re-encrypt them  
  and then copy the re-encrypted files back into the game, overriding the game's existing encrypted files.  
  Using this, the encrypted parts of the game can easily be modified (e.g. changing cursor images, etc.).

**Examples:**

If Teardown is installed at `D:\SteamLibrary\steamapps\common\Teardown\`

To **Decrypt**, run: `TDecryptor.exe "D:\SteamLibrary\steamapps\common\Teardown\" -D`.  
Decrypted files are then written to `D:\SteamLibrary\steamapps\common\Teardown\DECRYPTED\`.
To **Re-encrypt**, run: `TDecryptor.exe "D:\SteamLibrary\steamapps\common\Teardown\" -E`.

# Compiling From Source

To compile from source, you need a compiler with C++ 20 support and make sure the `codecvt` depreciation preprocessor definitions are set.

Other VS versions and/or compilers might work but I have not tested this.

# Notes

TDecryptor was tested to be fully working with the latest Teardown version at the time it was created (1.5.2).

If the decryptor breaks in a future update, I will likely not update it myself, though I might accept pull requests.

The code quality is probably not perfect but it works, so...
