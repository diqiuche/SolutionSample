#include <iostream>
#include <sstream>
#include <map>
#include <signal.h>
#include <stdio.h>
#include <memory>
#include <vector>
#include <set>
#include <assert.h>

using namespace std;

char * mystrcpy(char *strDest, const char *strSrc)
{
	assert(strDest != NULL && strSrc != NULL);

	char *temp = strDest;
	while ( (*strDest++ = *strSrc++) != '\0' )
		NULL;

	return temp;
}

int mystrlen(const char *str)
{
	assert(str != NULL);
	int len = 0;
	while ( *str++ != '\0' )
	{
		len++;
	}

	return len;
}

void BubbleSort(vector<int> &data)
{
	int count = data.size();
	bool tag = false;

	for ( int i = 0; i < count; i++ )
	{
		tag = false;
		for ( int j = 0; j < count-i-1; j++ )
		{
			if ( data[j] > data[j+1] )
			{
				tag = true;
				int temp = data[j];
				data[j] = data[j+1];
				data[j+1] = temp;
			}
		}

		if ( !tag )
		{
			break;
		}
	}
}

void QuickSort( vector<int> &data, int left, int right )
{
	if ( left < right )
	{
		int index = data[left];
		int low = left;
		int high = right;

		while ( low < high )
		{
			while ( low < high && index < data[high] ) high--;
			data[low] = data[high];
			while ( low < high && index > data[low] ) low++;
			data[high] = data[low];
		}

		data[low] = index;

		QuickSort(data, left, low-1);
		QuickSort(data, low+1, right);
	}
}


void SelectSort( vector<int> &data )
{
	int count = data.size();
	for ( int i = 0; i < count; i++ )
	{
		int min = data[i]; 
		int index = i;
		for ( int j = i+1; j < count; j++ )
		{
			if ( min > data[j] )
			{
				min = data[j];
				index = j;
			}
		}

		if ( i != index )
		{
			int temp = data[i];
			data[i] = data[index];
			data[index] = temp;
		}
	}
}

class String
{
public:
	String(const char *str = NULL);
	String(const String &other);
	virtual ~String(void);
	String & operator =(const String &other);

private:
	char *m_data;
};

String::String(const char *str)
{
	if ( str == NULL )
	{
		m_data = new char[1];
		m_data[0] = '\0';
	} else
	{
		m_data = new char[strlen(str)+1];
		if ( m_data != NULL )
		{
			strcpy(m_data, str);
		}
	}
}

String::String(const String &other)
{
	m_data = new char[strlen(other.m_data)+1];
	if ( m_data != NULL )
	{
		strcpy(m_data, other.m_data);
	}
}

String::~String(void)
{
	if ( m_data != NULL )
	{
		delete[] m_data;
	}
}

String& String::operator=(const String &other)
{
	if ( this == &other )
	{
		return *this;
	} 

	delete[] m_data;

	m_data = new char[strlen(other.m_data)+1];
	if ( m_data != NULL )
	{
		strcpy(m_data, other.m_data);
	}

	return *this;
}


int main()
{

	getchar();
	return 0;
}

