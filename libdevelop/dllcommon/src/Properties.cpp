#include <fstream>
#include <sstream>

#include "Properties.h"
#include "Applog.h"

namespace itstation {
	namespace common {

		bool SplitString(const string& str, const string& delim, vector<string>& result) {
			string::size_type pos = 0;
			string::size_type length = str.length();
			string elt;

			while ( pos < length ) 
			{
				if ( delim.find(str[pos]) != string::npos )
				{
					if (elt.length() > 0) 
					{
						result.push_back(elt);
						elt = "";
					}
				} else
				{
					elt=elt+str[pos];
				}

				if ( ++pos == length )
				{
					if (elt.length() > 0) 
					{
						result.push_back(elt);
						elt = "";
					}
					break;
				}
			} 

			return true;
		}

		Properties::Properties() 
		{
		}

		Properties::Properties(const string& file) 
		{
			Load(file);
		}

		Properties::Properties(const Properties& other) 
			: parameters_(other.parameters_)
		{
		}

		Properties::~Properties() {
			parameters_.clear();
		}

		string Properties::GetProperties(const string& group, const string& key) {
			map<string, PropertiesDict>::iterator iter;

			// Find group whether exist
			iter = parameters_.find(group);
			if (iter != parameters_.end()) {
				// Find key
				PropertiesDict::iterator pro_iter;
				pro_iter = iter->second.find(key);
				if (pro_iter != iter->second.end()) {
					return pro_iter->second;
				}
			}
			return string();
		}

		int Properties::GetPropertiesAsInt(const string& group, const string& key) {
			string str = GetProperties(group, key);

			int val = 0;
			istringstream in(str);
			if (!(in >> val) || !in.eof()) {
				APP_LOG(Applog::LOG_WARNING)
					<< "Numeric property [" << key 
					<< "] set to non-numeric value, defaulting to 0";
				return 0;
			}
			return val;
		}

		float Properties::GetPropertiesAsFloat(const string& group, const string& key) {
			string str = GetProperties(group, key);

			float val = 0;
			istringstream in(str);
			if (!(in >> val) || !in.eof()) {
				APP_LOG(Applog::LOG_WARNING)
					<< " Numeric property [" << key 
					<< "] set to non-numeric value, defaulting to 0";
				return 0.0;
			}
			return val;
		}

		double Properties::GetPropertiesAsDouble(const string& group, const string& key) {
			string str = GetProperties(group, key);

			double val = 0;
			istringstream in(str);
			if (!(in >> val) || !in.eof()) {
				APP_LOG(Applog::LOG_WARNING)
					<< " Numeric property [" << key 
					<< "] set to non-numeric value, defaulting to 0.";

				return 0.0;
			}
			return val;
		}

		int Properties::GetPropertiesList(const string& group, const string& key, 
			vector<string>* result) {
				string str = GetProperties(group, key);
				if (str != string()) {
					if (!SplitString(str, ", \t\r\n", *result)) {
						APP_LOG(Applog::LOG_WARNING)
							<< " mismatched quotes in property "
							<< key << "'s value, returning default value";
					}
					return result->size();
				}
				return 0;
		}

		int Properties::GetPropertiesAsIntList(const string& group, const string& key,
			vector<int>* result) {
				vector<string> value;
				if (GetPropertiesList(group, key, &value) == 0) {
					return 0;
				}

				for (vector<string>::iterator iter = value.begin();
					iter != value.end(); ++iter) {
						int val;
						istringstream in(*iter);
						if (!(in >> val) || !in.eof()) {
							APP_LOG(Applog::LOG_WARNING)
								<< " numeric property " << key
								<< " set to non-numeric value";

							result->push_back(0);
						}
						else {
							result->push_back(val);
						}
				}

				return result->size();
		}

		int Properties::GetPropertiesAsFloatList(const string& group, const string& key,
			vector<float>* result) {
				vector<string> value;
				if (GetPropertiesList(group, key, &value) == 0) {
					return 0;
				}

				for (vector<string>::iterator iter = value.begin();
					iter != value.end(); ++iter) {
						float val;
						istringstream in(*iter);
						if (!(in >> val) || !in.eof()) {
							APP_LOG(Applog::LOG_WARNING)
								<< " numeric property " << key
								<< " set to non-numeric value";

							result->push_back(0.);
						}
						else {
							result->push_back(val);
						}
				}

				return result->size();
		}

		int Properties::GetPropertiesAsDoubleList(const string& group, const string& key,
			vector<double>* result) {
				vector<string> value;
				if (GetPropertiesList(group, key, &value) == 0) {
					return 0;
				}

				for (vector<string>::iterator iter = value.begin();
					iter != value.end(); ++iter) {
						double val;
						istringstream in(*iter);
						if (!(in >> val) || !in.eof()) {
							APP_LOG(Applog::LOG_WARNING)
								<< " numeric property " << key
								<< " set to non-numeric value";

							result->push_back(0.);
						}
						else {
							result->push_back(val);
						}
				}

				return result->size();
		}

		PropertiesDict Properties::GetPropertiesDictForGroup(const string& group) {
			map<string, PropertiesDict>::iterator iter;

			// Find group whether exist
			iter = parameters_.find(group);
			if (iter != parameters_.end()) {
				return iter->second;
			}
			return PropertiesDict();
		}

		void Properties::Load(const string& file) {
			ifstream in(file.c_str());
			if (!in) {
				// Error handler
				string reason = "File path isn't exist. file = ";
				APP_LOG(Applog::LOG_ERROR)<<reason<<file;
			}
			string line;
			while (getline(in , line)) {
				ParseLine(line);
			}
		}

		void Properties::ParseLine(const string& line) {
			string group;
			string key;
			string value;

			// Parse current content of state.
			enum ParseState {NORMAL, GROUP, KEY, VALUE};
			ParseState state = NORMAL;

			string whitespace;
			bool finished = false;
			for (string::size_type i = 0; i < line.size(); i++) 
			{
				char c = line[i];
				switch (state) 
				{
				case NORMAL:
					{
						switch (c) 
						{
						case ' ':
						case '\t':
						case '\r':
						case '\n':
							break;

						case '#':
							finished = true;
							break;

						case '[':
							state = GROUP;
							break;

						default:
							key= key + (c);
							state = KEY;
							break;
						}
						break;
					}

				case GROUP:
					{
						switch (c) 
						{
						case '\\':
							if (i < line.length() - 2) 
							{
								c = line[++i];
								switch (c) {
								case '\\':
									group += whitespace;
									whitespace.clear();
									group=group + c;
									break;

								case ' ':
									if (group.length() != 0) 
									{
										whitespace += c;
									}
									break;

								default:
									group += whitespace;
									whitespace.clear();
									group += '\\';
									group += c;
									break;
								}
							}
							else {
								group += whitespace;
								whitespace.clear();
								group += c;
							}
							break;

						case ' ':
						case '\t':
						case '\r':
						case '\n':
							if (group.length() != 0) 
							{
								whitespace += c;
							}
							break;

						case ']':
							finished = true;
							break;

						default:
							group += whitespace;
							whitespace.clear();
							group += c;
							break;

						}
						break;
					}

				case KEY:
					{
						switch (c) 
						{
						case '\\':
							if (i < line.length() - 1) 
							{
								c = line[++i];
								switch (c) 
								{
								case '\\':
								case '=':
									key += whitespace;
									whitespace.clear();
									key += c;
									break;

								case ' ':
									if (key.length() != 0) 
									{
										whitespace += c;
									}
									break;

								default:
									key += whitespace;
									whitespace.clear();
									key += '\\';
									key += c;
									break;
								}
							} else 
							{
								key += whitespace;
								whitespace.clear();
								key += c;
							}
							break;

						case ' ':
						case '\t':
						case '\r':
						case '\n':
							if (key.length() != 0) 
							{
								whitespace += c;
							}
							break;

						case '=':
							whitespace.clear();
							state = VALUE;
							break;

						default:
							key += whitespace;
							whitespace.clear();
							key += c;
							break;
						}
						break;
					}
				case VALUE:
					{
						switch (c) 
						{
						case '\\':
							if (i < line.length() - 1) 
							{
								c = line[++i];
								switch (c) 
								{
								case '\\':
									value += whitespace;
									whitespace.clear();
									value += c;
									break;

								case ' ':
									if ( value.length() != 0 )
									{
										whitespace += c;
									}
									break;

								default:
									value += whitespace;
									whitespace.clear();
									value += '\\';
									value += c;
									break;
								}
							} else 
							{
								value += whitespace;
								whitespace.clear();
								value += c;
							}
							break;

						case ' ':
						case '\t':
						case '\r':
						case '\n':
							if (value.length() != 0) 
							{
								whitespace += c;
							}
							break;

						default:
							value += whitespace;
							whitespace.clear();
							value += c;
							break;
						}
						break;
					}

					default:
						break;
				}
				if (finished) 
				{
					break;
				}
			}

			if ( state == GROUP ) {
				if (group.length() == 0 || !finished) 
				{
					// Error handler
					string reason = "config file struct[group] is wrong";
					APP_LOG(Applog::LOG_ERROR)<<reason;
					exit(1);
				} else
				{
					AddGroup(group);
				}
				return;
			}

			if ( state == KEY  || (state == VALUE && key.length() == 0) ) 
			{
				// Error handler
				string reason = "config file struct[key/value] is wrong";
				APP_LOG(Applog::LOG_ERROR)<<reason;
				exit(1);
			} else 
			{
				AddProperties(key, value);
			}
			return;
		}

		void Properties::AddGroup(const string& group) {
			map<string, PropertiesDict>::iterator iter;

			// Find group whether exist
			iter = parameters_.find(group);
			if (iter != parameters_.end()) {
				current_properties_dict_ = &(iter->second);
			} else {
				// Add a new ProperitesDict.
				PropertiesDict properties;
				pair<string, PropertiesDict> properties_dict_pair(group, properties);
				parameters_.insert(properties_dict_pair);

				current_properties_dict_ = &(parameters_[group]);
			}
		}

		void Properties::AddProperties(const string& key, const string& value) {
			pair<string, string> properties_pair(key, value);
			current_properties_dict_->insert(properties_pair);
		}
	}
}