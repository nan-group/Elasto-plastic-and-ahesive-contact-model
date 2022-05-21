#include "CParametersList.h"

#include <vector>
#include <algorithm>

using namespace std;

const string CParametersList::JOIN_STRING = "~_~_~";

void CParametersList::addElastoPlasticAdhesiveParameters(const string & key1,
                                            const string &key2,
                                            CElastoPlasticAdhesiveParameters ElastoPlasticAdhesiveParameters)
{
    vector<string> keys;
    keys.push_back(key1);
    keys.push_back(key2);

    //Use STL sort to put them in alphabetical order.
    sort(keys.begin(), keys.end());

    string nKey = keys[0] + JOIN_STRING + keys[1];
    insert(pair<string, CElastoPlasticAdhesiveParameters>(nKey, ElastoPlasticAdhesiveParameters));
}

CElastoPlasticAdhesiveParameters CParametersList::getElastoPlasticAdhesiveParameters(const string & key1,
                                                       const string &key2)
{

    string nKey;
	if( 0 > key1.compare(key2) )
	{
		nKey=key1 + JOIN_STRING + key2;
	}
	else
	{
		nKey=key2+ JOIN_STRING + key1;
	}

    map<string, CElastoPlasticAdhesiveParameters>::iterator it;
    if((it = find(nKey)) != end() )
    { 
        return it->second;
    }
    else
    {
        return CElastoPlasticAdhesiveParameters();
    }

}
