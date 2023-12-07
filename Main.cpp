
#include <filesystem>
#include <string>
#include <fstream>
#include <locale>
#include <codecvt>
#include <format>


/*
TDecryptor: An Automatic Teardown .tde File Decryptor
How to use:
- Open a command line (CMD) and navigate to the directory where TDecryptor.exe is located (e.g. TDecryptor/Build/Release/)
- Type in: TDecryptor.exe "path/to/your/Teardown/installation/" and hit enter
- The program will now create a directory called "DECRYPTED" at the root of your Teardown installation
  and proceed to find, decrypt and dump all encrypted .tde files into that directory.
  It also keeps the directory structure of these files, allowing for easy browsing and finding specific files.

Example:
If Teardown is installed at D:\SteamLibrary\steamapps\common\Teardown\
Run: TDecryptor.exe "D:\SteamLibrary\steamapps\common\Teardown\"
Decrypted files are then written to D:\SteamLibrary\steamapps\common\Teardown\DECRYPTED\
*/


#define print( t, ... ) printf( (std::format( t, __VA_ARGS__ ) + "\n").c_str() );

using convert_type_utf8utf16 = std::codecvt_utf8_utf16<wchar_t>;
std::wstring_convert<convert_type_utf8utf16, wchar_t> wstring_converter;
std::wstring string_to_wstring( const std::string& str ) {
	return wstring_converter.from_bytes( str );
}

// Keys from https://github.com/lyhyl/TDEDecrypt
const char* const Key1 = "599Cc51887A8cb0C20F9CdE34cf9e0A535E5cAd1C26c7b562596ACC207Ae9A0bfB3E3161f31af5bEf1c2f064b3628174D83BF6E0739D9D6918fD9C2Eba02D5aD\0";
const char* const Key2 = "0C3b676fe8d7188Cde022F71632830F36b98b181aD48Fed160006eA59\0";

std::string readFile( const std::string& path );
void writeFile( const std::string& path, const std::string& data );
void createDirectoryIfNotExists( const std::string& path );
void decrypt( const std::string& in, std::string& out );

int main( int argc, const char** argv ) {
	if ( argc != 2 ) {
		print( "Expected 1 command line argument (Path to game installation)" );
		print( "Example usage: TDecryptor.exe \"D:\\SteamLibrary\\steamapps\\common\\Teardown\\\"" );
		return 0;
	}
	std::string TD( argv[argc - 1] );
	// Remove quotes if path is passed as "path\to\TD"
	if ( TD[0] == '\"' ) {
		TD.erase( 0 );
	}
	if ( TD[TD.size() - 1] == '\"' ) {
		TD.erase( TD.size() - 1 );
	}
	// Add a \ at the end if it is missing
	if ( TD[TD.size() - 1] != '\\' ) {
		TD.push_back( '\\' );
	}

	// Make sure there's not an existing output directory interfering with anything
	if ( std::filesystem::is_directory( TD + "DECRYPTED\\" ) ) {
		std::filesystem::remove_all( string_to_wstring( TD + "DECRYPTED\\" ) );
		std::filesystem::remove( string_to_wstring( TD + "DECRYPTED\\" ) );
	}

	// First, find all TDE files and their relative paths
	std::vector<std::string> vTDEFiles;
	for ( const auto& entry : std::filesystem::recursive_directory_iterator(string_to_wstring(TD)) ) {
		const std::filesystem::path& p = entry.path();
		if ( std::filesystem::is_regular_file(p) ) {
			if ( p.extension() == ".tde" ) {
				vTDEFiles.emplace_back( p.string().substr( TD.size() ) );
			}
		}
	}

	// Then, make sure each path exists in the output directory and decrypt the files
	std::string decrypted_base = "DECRYPTED\\";
	createDirectoryIfNotExists( TD + decrypted_base );
	for ( const std::string& path : vTDEFiles ) {
		// Handle subdirectories & get decrypted file name
		std::string decryptedFileName;
		{
			size_t offset = path.find( '\\' );
			while ( offset != std::string::npos ) {
				std::string target = TD + decrypted_base + path.substr( 0, offset );
				createDirectoryIfNotExists( target );
				offset = path.find( '\\', offset + 1 );
			}
			offset = path.find_last_of( '\\' );
			if ( offset != std::string::npos ) {
				decryptedFileName = path.substr( offset + 1 );
				decryptedFileName = decryptedFileName.substr( 0, decryptedFileName.size() - 4 );
			} else {
				decryptedFileName = path;	// No subdirectories?
			}
		}

		std::string pathWithoutFile = path.substr( 0, path.find_last_of( '\\' ) + 1 );
		std::string encryptedData = readFile( TD + path );
		std::string decryptedData;
		decrypt( encryptedData, decryptedData );
		std::string decryptedPath = decrypted_base + pathWithoutFile + decryptedFileName;
		writeFile( TD + decryptedPath, decryptedData );

		print( "Decrypted file '{}' into '{}'", path, decryptedPath );
	}
	return 0;
}

std::string readFile( const std::string& path ) {
	try {
		std::string fileData;
		const auto size = std::filesystem::file_size( string_to_wstring( path ) );
		fileData.resize( size );
		std::ifstream file;
		file.exceptions( std::ifstream::failbit | std::ifstream::badbit );
		file.open( string_to_wstring( path ), std::ios::binary );
		file.read( fileData.data(), fileData.size() );
		file.close();
		return std::move(fileData);
	}  catch( std::ifstream::failure& e ) {
		print( "Failed to read a file: {}: {}", path, e.what() );
	} catch( std::filesystem::filesystem_error& e ) {
		print( "Failed to read a file: {}: {}", path, e.what() );
	} catch( ... ) {
		print( "Failed to read a file: %s: Unknown exception", path.c_str() );
	}
	return "";
}

void writeFile( const std::string& path, const std::string& data ) {
	try {
		std::ofstream file;
		file.exceptions( std::ofstream::failbit | std::ofstream::badbit );
		file.open( string_to_wstring( path ), std::ios::binary );
		file.write( data.c_str(), data.size() );
		file.close();
	}  catch( std::ifstream::failure& e ) {
		print( "Failed to write a file: {}: {}", path, e.what() );
	} catch( std::filesystem::filesystem_error& e ) {
		print( "Failed to write a file: {}: {}", path, e.what() );
	} catch( ... ) {
		print( "Failed to write a file: %s: Unknown exception", path.c_str() );
	}
}

void createDirectoryIfNotExists( const std::string& path ) {
	if ( !std::filesystem::is_directory( string_to_wstring(path) ) ) {
		std::filesystem::create_directory( string_to_wstring(path) );
	}
}

// Decryption code, slightly modified, from https://github.com/lyhyl/TDEDecrypt
void decrypt( const std::string& in, std::string& out ) {
	out.reserve( in.size() );

	size_t i = 0;
	for ( char dat : in ) {
		out.push_back( dat ^ Key1[i & 0x7F] ^ Key2[i % 57] );
		i++;
	}
}
