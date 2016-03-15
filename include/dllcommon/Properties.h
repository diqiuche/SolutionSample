//
// Copyright Shanghai Flagship Prime Brokerage Technology LTD  2011. 
// All Rights Reserved.
// Room 502, Building 2, No.1690, 
// CaiLun Road, Zhangjiang High-tech Park, Shanghai.
//
// The copyright to the computer program(s) herein
// is the property of Shanghai Flagship Prime Brokerage Technology, china.
// The program(s) may be used and/or copied only with the
// written permission of Shanghai Flagship Prime Brokerage Technology 
// or in accordance with the terms and conditions stipulated in the
// agreement/contract under which the program(s) have
// been supplied. This copyright notice must not be
// removed.
//

#ifndef EYEGLE_COMMON_PROPERTIES_H_
#define EYEGLE_COMMON_PROPERTIES_H_

#include <map>
#include <vector>
#include "Global.h"

using namespace std;

namespace itstation {
	namespace common {

		typedef map<string, string> PropertiesDict;

		// Read Properites config file.
		class COMMON_API Properties 
		{
		public:
			// Default constructor.
			Properties();
			Properties(const string& file);
			// Copy constructor
			Properties(const Properties& other);
			~Properties();

			// Through the properties group and key obtain a string value.
			// @param group: properties group name.
			// @param key: properties key name.
			// @return: return string type value.
			// @exception: if group or key isn't exist, then return string()
			string GetProperties(const string& group, const string& key);

			// Through the properties group and key obtain a int value.
			// @param group: properties group name.
			// @param key: properties key name.
			// @return: return int type value.
			// @exception: if group or key isn't exist, then return 0
			int GetPropertiesAsInt(const string& group, const string& key);

			// Through the properties group and key obtain a float value.
			// @param group: properties group name.
			// @param key: properties key name.
			// @return: return float type value.
			// @exception: if group or key isn't exist, then return 0
			float GetPropertiesAsFloat(const string& group, const string& key);

			// Through the properties group and key obtain a double value.
			// @param group: properties group name.
			// @param key: properties key name.
			// @return: return double type value.
			// @exception: if group or key isn't exist, then return 0
			double GetPropertiesAsDouble(const string& group, const string& key);

			// Through the properties group and key obtain a vector<string> value.
			// @param group: properties group name.
			// @param key: properties key name.
			// @param(out) result: output obtain of value.
			// @return: if group and key is exist, return properties value of
			//			vector<string> size. else return 0.
			int GetPropertiesList(const string& group, const string& key,
				vector<string>* result);

			// Through the properties group and key obtain a vector<int> value.
			// @param group: properties group name.
			// @param key: properties key name.
			// @param(out) result: output obtain of value.
			// @return: if group and key is exist, return properties value of
			//			vector<int> size. else return 0.
			int GetPropertiesAsIntList(const string& group, const string& key,
				vector<int>* result);

			// Through the properties group and key obtain a vector<float> value.
			// @param group: properties group name.
			// @param key: properties key name.
			// @param(out) result: output obtain of value.
			// @return: if group and key is exist, return properties value of
			//			vector<float> size. else return 0.
			int GetPropertiesAsFloatList(const string& group, const string& key,
				vector<float>* result);

			// Through the properties group and key obtain a vector<double> value.
			// @param group: properties group name.
			// @param key: properties key name.
			// @param(out) result: output obtain of value.
			// @return: if group and key is exist, return properties value of
			//			vector<double> size. else return 0.
			int GetPropertiesAsDoubleList(const string& group, const string& key,
				vector<double>* result);


			PropertiesDict GetPropertiesDictForGroup(const string& group);
			// Load properties file.
			// @param file: file name (include file path)
			void Load(const string& file);

		private:
			// Parse config file a line
			// @param line: properties file a line.
			void ParseLine(const string& line);

			// Add a new properties group to parameters_. if group is exist that
			// wouldn't add.
			// @param group: new add group name.
			void AddGroup(const string& group);

			// Add a new properties to specific PropertiesDict. if key is exist. then
			// wouldn't add.
			// @param key: new add key name.
			// @param value: key's value.
			void AddProperties(const string& key, const string& value);

			PropertiesDict *current_properties_dict_;

			// Parameters of map.
			map<string, PropertiesDict> parameters_;
		};

	}
}
#endif
