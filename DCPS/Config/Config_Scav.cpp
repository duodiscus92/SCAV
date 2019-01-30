
//=============================================================================
/**
 *  @file    Config_Scav.cpp
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

#include "Config_Scav.h"
//#include "test_config.h"
//#include "Config_Test.h"
#include "ace/Configuration_Import_Export.h"
#include "ace/OS_NS_ctype.h"
#include "ace/OS_NS_errno.h"
#include "ace/OS_NS_stdio.h"
#include "ace/OS_NS_string.h"
#include "ace/OS_NS_unistd.h"
#include "ace/streams.h"
#include <string>

ACE_Configuration_Heap cf;

// Open and import .ini file
int Config_Scav::ImportConfigFile(const ACE_TCHAR* IniFileName)
{
   int status;

    if ((status = cf.open ()) != 0)
      ACE_ERROR ((LM_ERROR,
                  ACE_TEXT ("ACE_Configuration_Heap::open returned %d\n"),
                  status));

    ACE_Ini_ImpExp import (cf);

    status = import.import_config (IniFileName);
    if (status != 0) {
      ACE_ERROR ((LM_ERROR, ACE_TEXT ("%p: %s\n"),
                  ACE_TEXT ("Config file failed"),
                  IniFileName));
    }
    else
      ACE_DEBUG ((LM_DEBUG, ACE_TEXT ("%s imported\n"), IniFileName));

   return status;
}

// Verify that that actual number of section is equal to expected number of section
int  Config_Scav::TestNumberOfSectionsFromConfigFile(const int NumberOfExpectedSections, int& NumberOfSections)
{
   int status;
   ACE_TString sect_name;
   const ACE_Configuration_Section_Key &root = cf.root_section ();

   for (NumberOfSections = 0;
        (status = cf.enumerate_sections (root, NumberOfSections, sect_name)) == 0;
        ++NumberOfSections) {
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("Found section Nr: %d Named: %s\n"),
                  NumberOfSections + 1,
		  sect_name.c_str ()));
   }
   status =  (NumberOfSections) == NumberOfExpectedSections;

   if (status == 0)
       ACE_ERROR ((LM_ERROR,
                   ACE_TEXT ("Found %d section(s), expected %d section(s)\n"),
                   NumberOfSections,
		   NumberOfExpectedSections));
   return status;
}

// Verify that that a given setion name exists
bool Config_Scav::IsDefinedSectionIntoConfigFile(const ACE_TCHAR *section)
{
   int status;
   ACE_TString sect_name;
   const ACE_Configuration_Section_Key &root = cf.root_section ();

   for (int NumberOfSections = 0;
        (status = cf.enumerate_sections (root, NumberOfSections, sect_name)) == 0;
        ++NumberOfSections) {
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("Found section name: %s\n"),
		  sect_name.c_str ()));
      //if (!strcmp(section, sect_name.c_str()) )
      if (ACE_TEXT(section) ==  sect_name )
	return true;
   }

   ACE_ERROR ((LM_ERROR,
               ACE_TEXT ("Section %s not found\n"),
               section));
   return false;
}

// Verify that a key in a given section exists
bool Config_Scav::IsDefinedKeyInSectionIntoConfigFile(const ACE_TCHAR *section, const ACE_TCHAR *key)
{
   int index = 0, status;
   ACE_TString val_name;
   ACE_Configuration::VALUETYPE val_type;
   ACE_Configuration_Section_Key sect1;
   const ACE_Configuration_Section_Key &root = cf.root_section ();

   if (cf.open_section (root, section, 0, sect1) != 0){
      ACE_ERROR ((LM_ERROR, ACE_TEXT ("Failed to open section: %s\n"),
                section));
      return false;
   }

   while ((status = cf.enumerate_values (sect1, index, val_name, val_type)) == 0) {
      ACE_DEBUG ((LM_DEBUG,
                 ACE_TEXT ("Enumerated %s, type %d\n"),
                 val_name.c_str (),
                 val_type));
      if (val_name == ACE_TEXT (key)) {
	ACE_DEBUG ((LM_DEBUG, ACE_TEXT("Found key %s\n"),
                  val_name.c_str()));
        return true;
      }
      ++index;
  }
  ACE_DEBUG((LM_DEBUG, ACE_TEXT("Key %s not found\n"),
	   key));
  return false;
}

// Get an integer config parameter for a given section and key
int Config_Scav::GetStringFromConfigFile(const ACE_TCHAR *section, const ACE_TCHAR *key, ACE_TCHAR *StringValue)
{
   int index = 0;
   ACE_TString string_value, val_name;
   ACE_Configuration::VALUETYPE val_type;
   ACE_Configuration_Section_Key sect1;
   const ACE_Configuration_Section_Key &root = cf.root_section ();

   // test id section exists
   if(!Config_Scav::IsDefinedSectionIntoConfigFile(ACE_TEXT(section)) )
      return -1;

   // tezst if key in section exists
   if (!Config_Scav::IsDefinedKeyInSectionIntoConfigFile(ACE_TEXT(section), ACE_TEXT(key)))
      return -2;

   // open section
   if (cf.open_section (root, section, 0, sect1) != 0){
      ACE_ERROR ((LM_ERROR, ACE_TEXT ("Failed to open section: %s\n"),
                section));
      return -3;
   }

   // search for key
   while (cf.enumerate_values (sect1, index, val_name, val_type) == 0) {
      ACE_DEBUG ((LM_DEBUG,
                 ACE_TEXT ("Enumerated %s, type %d\n"),
                 val_name.c_str (),
                 val_type));
      if (val_name == ACE_TEXT (key)) {
	ACE_DEBUG ((LM_DEBUG, ACE_TEXT("Found key %s\n"),
                  val_name.c_str()));
        // key found, get string
        if (cf.get_string_value (sect1,
                                 ACE_TEXT(key),
                                 string_value) == 0) {
	   // return the string
	   ACE_OS::strcpy(StringValue, string_value.c_str());
        }
        return 0;
      }
      ++index;
   } // end while
   return -4;
}

// Get an integer config parameter for a given section and key
int Config_Scav::GetIntFromConfigFile(const ACE_TCHAR *section, const ACE_TCHAR *key, int *IntValue)
{
   const char *pc;
   char StringValue[80];
   int status;

   // test id section exists
   if(!Config_Scav::IsDefinedSectionIntoConfigFile(ACE_TEXT(section)) )
      return -1;

   // test if key in section exists
   if (!Config_Scav::IsDefinedKeyInSectionIntoConfigFile(ACE_TEXT(section), ACE_TEXT(key)))
      return -2;

   // Get the string value 
   if(!(status = Config_Scav::GetStringFromConfigFile(ACE_TEXT(section), ACE_TEXT(key), StringValue)) ) {
      // test is string is a number
      pc = StringValue;
      while (*pc) {
         if(!isdigit(*pc) && *pc != '-' && *pc != '+') {
	    ACE_ERROR ((LM_ERROR, ACE_TEXT("Value type is not an integer %s\n"),
	 	       StringValue) );
	    return -4;
         }
         pc++;
      }
      // convert string into integer
      *IntValue = atoi(StringValue);
      return 0;
   }
   return status;
}

// Get a double config parameter for a given section and key
int Config_Scav::GetDoubleFromConfigFile(const ACE_TCHAR *section, const ACE_TCHAR *key, double *RealValue)
{
   char StringValue[80], *endptr;
   int status;

   // test id section exists
   if(!Config_Scav::IsDefinedSectionIntoConfigFile(ACE_TEXT(section)) )
      return -1;

   // test if key in section exists
   if (!Config_Scav::IsDefinedKeyInSectionIntoConfigFile(ACE_TEXT(section), ACE_TEXT(key)))
      return -2;

   // Get the string value 
   if(!(status = Config_Scav::GetStringFromConfigFile(ACE_TEXT(section), ACE_TEXT(key), StringValue)) ) {
      // test is string is a number
      *RealValue = strtod(StringValue, &endptr);
      if(endptr == StringValue) {
	 ACE_ERROR ((LM_ERROR, ACE_TEXT("Value type is not an real number %s\n"),
	 	   StringValue) );
	    return -4;
      }
      return 0;
   }
   return status;
}

//#define TEST_CONFIG_SCAV
#ifdef TEST_CONFIG_SCAV
int main(/*int argc, char *argv[]*/)
{
   int IntValue, status, NumberOfSections;
   char StringValue[80];
   double DoubleValue;

   Config_Scav myconfig;

   myconfig.ImportConfigFile(ACE_TEXT("Config_Test.ini"));

   // test on number of existing sections
   myconfig.TestNumberOfSectionsFromConfigFile(3, NumberOfSections);

   // test if a given section exists
   myconfig.IsDefinedSectionIntoConfigFile(ACE_TEXT("others"));

   // test id a given key in a give section exists
   myconfig.IsDefinedKeyInSectionIntoConfigFile(ACE_TEXT("network"), ACE_TEXT("Delay"));

   // get the intzger value of a given key in a givzen section
   if(!(status = myconfig.GetIntFromConfigFile(ACE_TEXT("logger"), ACE_TEXT("SeekIndex"), &IntValue)) )
     cout << "Integer value is: " << IntValue << endl;
   else
     cout << "Integer value not found. Error: " << status << endl;

   // get the string value of a given key in a given section
   if(!(status = myconfig.GetStringFromConfigFile(ACE_TEXT("logger"), ACE_TEXT("SeekIndex"), StringValue)) )
     cout << "String value is: " << StringValue << endl;
   else
     cout << "String value not found. Error: " << status << endl;

   // get the double value of a given key in a given section
   if(!(status = myconfig.GetDoubleFromConfigFile(ACE_TEXT("logger"), ACE_TEXT("SeekIndex"), &DoubleValue)) )
     cout << "Double value is: " << DoubleValue << endl;
   else
     cout << "Double value not found. Error: " << status << endl;


  return 0;
}
#endif
