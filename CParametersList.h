#if !defined(cparameterslist_h)
#define cparameterslist_h

#include <map>
#include <string>

typedef struct
{
    double      m_nSurfaceEnergy;
    double      m_nYieldPressure;		
    double      m_nElasticStiffnessMinRatio;	
    double      m_nPlasticStiffnessRatio;
    double      m_nPlasticDampingRatio;
}CElastoPlasticAdhesiveParameters;

class CParametersList: public std::map<std::string, CElastoPlasticAdhesiveParameters>
{
    public:
        static const std::string JOIN_STRING;

        void addElastoPlasticAdhesiveParameters(const std::string & key1,
                              const std::string &key2,
                              CElastoPlasticAdhesiveParameters ElastoPlasticAdhesiveParametersParameters);

        CElastoPlasticAdhesiveParameters getElastoPlasticAdhesiveParameters(const std::string &key1,
                                          const std::string &key2);
};

#endif