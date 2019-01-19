//=============================================================================
/**
 *  @file    Config_Scav.h
 *
 *
 *    This is a .ini file reader based on 
 *    <ACE_Configuration> 
 *
 *  @author Jacques Ehrlich <jacques.ehrlich@djet.fr/
 *  @inspired from author Michael Searles <msearles@base16.com>
 *  @inspired from author Chris Hafey <chafey@stentor.com>
 *  @inspired from author Jerry D. Odenwelder Jr. <jerry.o@mindspring.com>
 */
//=============================================================================


#ifndef __CONFIG_SCAV_H
#define __CONFIG_SCAV_H

#include "ace/Configuration.h"

enum {
  ERROR_UNABLE_TO_IMPORT_CONFIG_FILE,
  ERROR_SECTION_NOT_FOUND,
  ERROR_KEY_NOT_FOUND,
  ERROR_VALUE_NOT_FOUND,
  ERROR_NOT_A_STRING,
  ERROR_NOT_AN_INTEGER,
  ERROR_NOT_A_DOUBLE
};

class Config_Scav
{
public:
  Config_Scav (void) { }
  ~Config_Scav (void) { }

  // Open and import .ini file
  int ImportConfigFile(const ACE_TCHAR* IniFileName);

  // Verify that that actual number of section is equal to expected number of section
  int TestNumberOfSectionsFromConfigFile(const int NumberOfExpectedSections, int& NumberOfSections);

  // Verify that that a given setion name exists 
  bool IsDefinedSectionIntoConfigFile(const ACE_TCHAR *section);

  // Verify that a key in a given section exists
  bool IsDefinedKeyInSectionIntoConfigFile(const ACE_TCHAR *section, const ACE_TCHAR *key);

  // Get a string  config parameter for a given section and key
  int GetStringFromConfigFile(const ACE_TCHAR *section, const ACE_TCHAR *key, ACE_TCHAR *StringValue);

  // Get an integer config parameter for a given section and key
  int GetIntFromConfigFile(const ACE_TCHAR *section, const ACE_TCHAR *key, int* IntValue);

  // Get a double (real) config parameter for a given section and key
  int GetDoubleFromConfigFile(const ACE_TCHAR *section, const ACE_TCHAR *key, double *RealValue);

private:
  //ACE_Configuration_Heap cf;

};

#endif /* __CONFIG_SCAV_H */
