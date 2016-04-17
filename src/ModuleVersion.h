/// @file ModuleVersion.h
/// @author rev

#pragma once

#include <string>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace RPW
{
	namespace Core
	{

		/// @b Usage:
		///
		/// @code
		/// ModuleVersion ver
		/// if (ver.Read(_T("mymodule"))) {
		///  // info is in ver, you can call GetValue to get variable info like
		///  CString s = ver.GetValue(_T("CompanyName"));
		/// }
		/// @endcode
		///
		/// You can also call the static fn DllGetVersion to get DLLVERSIONINFO.
		///
		/// @li Method 1 - Has problems in WinCE because szProg returns "\MyApp.exe" even if the app is on a storage card
		///
		/// @code
		/// TCHAR szProg[MAX_PATH * 2];
		/// ::GetModuleFileName(NULL, szProg, sizeof(szProg) / sizeof(TCHAR));
		/// ModuleVersion oVer;
		/// if (oVer.Read(szProg))
		/// {
		///  m_sCompanyName = oVer.GetValue(_T("CompanyName"));
		///  m_sProductName = oVer.GetValue(_T("ProductName"));
		///  m_sFileVersion = "Build:";
		///  m_sFileVersion += oVer.GetValue(_T("ProductVersion"));
		///  m_sFileVersion.Replace(_T(","), _T("."));
		///  m_sFileVersion.Remove(' ');
		///  m_sCopyright = oVer.GetValue(_T("LegalCopyright"));
		/// }
		/// @endcode
		///
		/// @li Method 2 - Get version of calling assembly (most common)
		///
		/// @code
		/// ModuleVersion oVer;
		/// if (oVer.Read(::GetModuleHandle(NULL)))
		/// {
		///  m_sCompanyName = oVer.GetValue(_T("CompanyName"));
		///      ...
		/// @endcode
		///
		/// @li Method 3 - Same result as Method 2
		///
		/// @code
		/// ModuleVersion oVer;
		/// if (oVer.Read()
		/// {
		///  m_sCompanyName = oVer.GetValue(_T("CompanyName"));
		///      ...
		/// @endcode
		///
		/// @brief Get module version via reflection
		/// @author Ron Wilson
		/// @author Paul DiLascia
		/// @see http://www.microsoft.com/msj/0498/c0498.aspx
		class ModuleVersion : public VS_FIXEDFILEINFO
		{
		public:
			ModuleVersion();
			virtual ~ModuleVersion();

			/// @brief Get the file version for a given module
			///
			/// Allocates storage for all info, fills "this" with
			/// VS_FIXEDFILEINFO, and sets codepage.
			bool Read();

			/// @brief Get the file version for a given module
			///
			/// Allocates storage for all info, fills "this" with
			/// VS_FIXEDFILEINFO, and sets codepage.
			bool Read( HMODULE hModule );

			/// @brief Get the file version for a given module
			///
			/// Allocates storage for all info, fills "this" with
			/// VS_FIXEDFILEINFO, and sets codepage.
			bool Read( const char* const modulename );

			/// @brief Get string file info
			/// @param szKeyName e.g. "CompanyName"
			std::string GetValue( const char* const szKeyName );

		protected:
			BYTE* m_pVersionInfo; ///< all version info

			/// @brief Storage for language character sets
			struct TRANSLATION
			{
				WORD langID;  ///< language ID
				WORD charset; ///< character set (code page)
			} m_translation;
		};

	}
}
