#define _CRT_SECURE_NO_WARNINGS
#include "CElastoPlasticAdhesive_V3_2_0.h"
#include "ICustomPropertyManagerApi_1_0.h"

#include <string.h>
#include <iostream>
#include <sstream>
#include <cstring>

using namespace NApi;
using namespace NApiCore;
using namespace NApiCm;
using namespace NApiHelpersV3_0_0;


namespace
{
    const std::string PREFS_FILE = "ContactModelPrefsFile.txt";
    const std::string ContactElastoPlasticAdhesiveResults = "ContactUserResults";   
}

void CElastoPlasticAdhesive_V3_2_0::getPreferenceFileName(char prefFileName[FILE_PATH_MAX_LENGTH])
{
    strncpy(prefFileName, PREFS_FILE.c_str(), FILE_PATH_MAX_LENGTH);
}

bool CElastoPlasticAdhesive_V3_2_0::isThreadSafe()
{
    // thread safe
    return true;
}

bool CElastoPlasticAdhesive_V3_2_0::usesCustomProperties()
{
    // Uses custom properties
    return true;
}

NApi::EPluginModelType CElastoPlasticAdhesive_V3_2_0::getModelType()
{
    return EPluginModelType::eBase;
}

NApi::EPluginExecutionChainPosition CElastoPlasticAdhesive_V3_2_0::getExecutionChainPosition()
{
    return EPluginExecutionChainPosition::eBasePos;
}

void CElastoPlasticAdhesive_V3_2_0::getGpuFileName(char gpuNameFile[FILE_PATH_MAX_LENGTH])
{

}

void CElastoPlasticAdhesive_V3_2_0::setFilePath(const char simFile[])
{
    std::string tempsimulationPath = simFile;
    tempsimulationPath = tempsimulationPath.substr(0, tempsimulationPath.rfind('/') + 1);

    simulationPath += tempsimulationPath;
    simulationPath += PREFS_FILE;

    tempsimulationPath.clear();
}

bool CElastoPlasticAdhesive_V3_2_0::setup(NApiCore::IApiManager_1_0& apiManager,
                   const char                 prefFile[],
                   char                       customMsg[NApi::ERROR_MSG_MAX_LENGTH])
{
    //Read in the preference file
    std::ifstream myinputfile(simulationPath);

    double      nSurfaceEnergy=0.0;
    double      nYieldPressure = 0.0;
    double      nElasticStiffnessMinRatio = 0.0;
    double      nPlasticStiffnessRatio = 0.0;
    double      nPlasticDampingRatio = 0.0;

    //strncpy(customMsg, simulationPath.c_str(), ERROR_MSG_MAX_LENGTH);// for debug of code

    if(!myinputfile)
    {
        strncpy(customMsg, "ContactModelPrefsFile.txt (input file) not found in the same folder of .dem file", ERROR_MSG_MAX_LENGTH);
        return false;
    }
    else
    {
        std::string description;
        std::string pairName;

        myinputfile >> description;        
        while (!myinputfile.eof())
        {
            myinputfile >> pairName >> nSurfaceEnergy >> nYieldPressure >> nElasticStiffnessMinRatio >> nPlasticStiffnessRatio >> nPlasticDampingRatio;
            
            std::string::size_type i(pairName.find(':'));
            std::string surfA = pairName.substr(0, i);
            pairName.erase(0, i + 1);
            std::string surfB = pairName;

            CElastoPlasticAdhesiveParameters elastoPlasticAdhesiveParams;
            elastoPlasticAdhesiveParams.m_nSurfaceEnergy = nSurfaceEnergy;
            elastoPlasticAdhesiveParams.m_nYieldPressure = nYieldPressure;
            elastoPlasticAdhesiveParams.m_nElasticStiffnessMinRatio = nElasticStiffnessMinRatio;
            elastoPlasticAdhesiveParams.m_nPlasticStiffnessRatio = nPlasticStiffnessRatio;
            elastoPlasticAdhesiveParams.m_nPlasticDampingRatio = nPlasticDampingRatio;
   
            m_ElastoPlasticAdhesiveParameters.addElastoPlasticAdhesiveParameters(surfA,
                                                                                 surfB,
                                                                                 elastoPlasticAdhesiveParams);           
        }
    }
    return true;;
}

bool CElastoPlasticAdhesive_V3_2_0::starting(NApiCore::IApiManager_1_0& apiManager, int numThreads)
{
	NApiCore::ICustomPropertyManagerApi_1_0* contactCustomPropertyManager = static_cast<NApiCore::ICustomPropertyManagerApi_1_0*>(apiManager.getApi(eContactCustomPropertyManager, 1, 0));
    m_ContactElastoPlasticAdhesiveResults_index = contactCustomPropertyManager->getPropertyIndex(ContactElastoPlasticAdhesiveResults.c_str());

	return true;
}

void CElastoPlasticAdhesive_V3_2_0::stopping(NApiCore::IApiManager_1_0& apiManager)
{
}

NApi::ECalculateResult CElastoPlasticAdhesive_V3_2_0::calculateForce(int threadID,
    const NCalcForceTypesV3_0_0::STimeStepData& timeStepData,
    const NCalcForceTypesV3_0_0::SDiscreteElement& element1,
    NApiCore::ICustomPropertyDataApi_1_0* elem1CustomProperties,
    const NCalcForceTypesV3_0_0::SDiscreteElement& element2,
    NApiCore::ICustomPropertyDataApi_1_0* elem2CustomProperties,
    NApiCore::ICustomPropertyDataApi_1_0* contactCustomProperties,
    NApiCore::ICustomPropertyDataApi_1_0* simulationCustomProperties,
    const NCalcForceTypesV3_0_0::SInteraction& interaction,
    const NCalcForceTypesV3_0_0::SContact& contact,
    NApiHelpersV3_0_0::CSimple3DVector& tangentialPhysicalOverlap,
    NCalcForceTypesV3_0_0::SContactResult& contactResults)

{
    CSimple3DVector zeroVector{ 0.0, 0.0, 0.0 };

    // unit vector from element 1 to the contact point
    CSimple3DVector unitCPVect = contact.contactPoint - element1.position;
    unitCPVect.normalise();

    // normal and tangential components of the relative velocity at the contact point
    CSimple3DVector relVel = element1.velocityAtContactPoint - element2.velocityAtContactPoint;
    CSimple3DVector relVel_n = unitCPVect * unitCPVect.dot(relVel);
    CSimple3DVector relVel_t = relVel - relVel_n;

    // Tangential Overlap
    CSimple3DVector nOverlap_t(tangentialPhysicalOverlap.getX(), tangentialPhysicalOverlap.getY(), tangentialPhysicalOverlap.getZ());

    // Equivalent radii & mass
    double nEquivRadius = element1.physicalRadius * element2.physicalRadius /
        (element1.physicalRadius + element2.physicalRadius);

    double nEquivMass = element1.mass * element2.mass /
        (element1.mass + element2.mass);

    // Effective Young's Modulus
    double nEquivYoungsMod = (2.0 * element1.shearModulus * element2.shearModulus) /
        (element2.shearModulus * (1.0 - element1.poisson) + element1.shearModulus * (1.0 - element2.poisson));

    // Effective Shear Modulus
    double nEquivShearMod = (element1.shearModulus * element2.shearModulus) /
        (element2.shearModulus * (2.0 - element1.poisson) + element1.shearModulus * (2.0 - element2.poisson));

    // Damping Ratio, following Eq. could be replaced by the equations suggested by Thorton et al. (2013)
    double dampingRatio = 0.0;
    if (interaction.coeffRest > 0.0)
    {
        double myLog = log(interaction.coeffRest);
        dampingRatio = -myLog / sqrt(myLog * myLog + PI * PI);
    }

    // Clear return values
    contactResults.normalForce = zeroVector;
    contactResults.usNormalForce = zeroVector;
    contactResults.tangentialForce = zeroVector;
    contactResults.usTangentialForce = zeroVector;
    //contactResults.additionalTorque1 = zeroVector; // not do anything or set torque to zero ?
    //contactResults.usAdditionalTorque1 = zeroVector;
    //contactResults.additionalTorque2 = zeroVector;
    //contactResults.usAdditionalTorque2 = zeroVector;

    // User input parameter
    CElastoPlasticAdhesiveParameters elastoPlasticAdhesiveParams = m_ElastoPlasticAdhesiveParameters.getElastoPlasticAdhesiveParameters(element1.type, element2.type);
    double nSurfaceEnergy = elastoPlasticAdhesiveParams.m_nSurfaceEnergy; // particle-particle/wall surface energy
    double py = elastoPlasticAdhesiveParams.m_nYieldPressure; // particle-particle/wall yield pressure
    double kemin = elastoPlasticAdhesiveParams.m_nElasticStiffnessMinRatio; // particle-particle/wall elastic stiffness ratio, kemin/(pi*R*py)
    double kp = elastoPlasticAdhesiveParams.m_nPlasticStiffnessRatio; // particle-particle/wall plastic stiffness ratio, kp/kemin
    double nPlasticDampingRatio = elastoPlasticAdhesiveParams.m_nPlasticDampingRatio; // particle-particle/wall plastic stiffness
 
    // scale ratio to its value
    kemin = kemin * PI * nEquivRadius * py;
    kp = kp * kemin;

    // Cache our custom properties, 0-maxOverlap, 1-isContact, 2-isPlastic, 3-normalForceMag. 2 and 3 are only for results-post and not necessary
    // More infos could be added and assigned as  4, 5... 
    const double* customResults = contactCustomProperties->getValue(m_ContactElastoPlasticAdhesiveResults_index);
    double* customResultsDelta = contactCustomProperties->getDelta(m_ContactElastoPlasticAdhesiveResults_index);
    
    // calcualte stiffness
    double ktRatio = 4.0 * nEquivShearMod / nEquivYoungsMod; // tangential stiffness ratio
    double kekcRatio = 1.0e10; // ke/kc, infinity (default value), only used for adhesive force calculation
    if (nSurfaceEnergy > 0.0)
    {
        kekcRatio = 0.92 * kemin / (pow((nEquivYoungsMod * nEquivYoungsMod * nEquivRadius * nEquivRadius * nSurfaceEnergy), 1.0 / 3.0)) - 0.3; 
    }
    
    // variables used in calculation
    CSimple3DVector F_n, F_nd, F_t, F_td, newF_n, newF_t;

    /*********************************************************************************/
    /******************************* Force Calculation *******************************/
    /*********************************************************************************/
    // basic parameters
    double fce = 1.5 * PI * nSurfaceEnergy * nEquivRadius;
    double f0 = 8.0 / 9.0 * fce;
    double fy0 = 1.0/6.0 * pow(PI * py, 3) * nEquivRadius / nEquivYoungsMod * nEquivRadius / nEquivYoungsMod;
    double fy = fy0 * sqrt(6.0 / 5.0 * kemin / (PI * nEquivRadius * py));
    double alpha_0 = f0 / kemin;
    double alpha_y = alpha_0 + fy / kemin;
    
    double alpha_max = customResults[0]; // previous timestep, t-delta_t
    if (alpha_max < contact.normalPhysicalOverlap) // loading/reloading stage
    {
        alpha_max = contact.normalPhysicalOverlap;  // using updated overlap for ke
    }

    if (alpha_max <= alpha_y) // minimum value - alpha_y
    {
        alpha_max = alpha_y; 
    }

    double ke= kemin * sqrt(alpha_max / alpha_y);

    double alpha_p = alpha_0;
    if (customResults[0] > contact.normalPhysicalOverlap)  // change alpha_p for unloading process with plastic deformation
    {
        alpha_p = (1.0 - kp / ke) * (alpha_max - alpha_y) + (1.0 - kemin / ke) * (alpha_y - alpha_0) + alpha_0;
    }

    double fcp = fce;
    if (nSurfaceEnergy > 0.0) 
    {
        double A = 16.0/27.0/(56.0/162.0*kekcRatio+17.0/162.0) ;
        fcp = -A + sqrt(A * A + 4.0* ke/kemin* (alpha_p/alpha_0 *A + 1.0));  
        fcp = 0.5 * fce * fcp;
    }

    double alpha_cp = alpha_p - fcp / ke;
    double alpha_c0 = alpha_p - 8.0 / 9.0 * fcp / ke;

    double normalForcePlastic = kp * (alpha_max - alpha_y) + fy; // replace contact.normalPhysicalOverlap by alpha_max
    double normalForceElastic = ke * (contact.normalPhysicalOverlap - alpha_p); 
    double normalForceAdhesive = -fcp + ke/kekcRatio * (alpha_cp - contact.normalPhysicalOverlap); //only used in "Calculation of linear cohesive Model"
    double normalForceDetach = -5.0 / 9.0 * fcp;

    if (contact.normalPhysicalOverlap > alpha_y) // using smaller dampingratio for the contact after yielding
    {
        dampingRatio = nPlasticDampingRatio * dampingRatio;
    }

    // start calculation
    double normalForceMag = 0.0;
    double kn = ke; // used in calculation of damping, change in different stage, =ke, kp, kc
    customResultsDelta[2] = 0.0 - customResults[2]; // set to plasticState to zero
    if (contact.normalPhysicalOverlap >= alpha_c0) 
    {
        //contact is established
        if (customResults[1] == 0.0)
        {
            customResultsDelta[1] = 1.0 - customResults[1];
        }

        /********************* Normal Force Calculation ********************************/
        // if (relVel_n.dot(unitCPVect) > 0.0 && (normalForcePlastic < normalForceElastic) && contact.normalPhysicalOverlap>alpha_y) // not used, as alpha_max used in normalForcePlastic 
        if (normalForcePlastic < normalForceElastic) 
        {
            normalForceMag = normalForcePlastic;
            customResultsDelta[0] = contact.normalPhysicalOverlap - customResults[0]; // set overlap as new overlap_max

            kn = kp;

            customResultsDelta[2] = 1.0 - customResults[2]; // set plastic state to 1
        }
        else
        {
            normalForceMag = normalForceElastic;
            kn = ke;
        }
        
        // Calculate normal force on element 1
        F_n = -unitCPVect * normalForceMag;


        // Calculate the normal damping force
        F_nd = unitCPVect * 2.0 * dampingRatio * sqrt(kn * nEquivMass) * relVel_n.length();
        // Are we in a loading situation?
        if (relVel_n.dot(unitCPVect) > 0.0)
        {
            F_nd = -F_nd;
        }     
        

        // Fill in parameters we were passed in
        newF_n = F_n + F_nd;
        contactResults.usNormalForce.setX(F_nd.getX());
        contactResults.usNormalForce.setY(F_nd.getY());
        contactResults.usNormalForce.setZ(F_nd.getZ());

        contactResults.normalForce.setX(newF_n.getX());
        contactResults.normalForce.setY(newF_n.getY());
        contactResults.normalForce.setZ(newF_n.getZ());


        /********************* Tangential Force Calculation ********************************/
        double kt = kn * ktRatio; 
        F_t = -nOverlap_t * kt;

        double ft2 = F_t.length();
        double fn2 = F_n.length() * interaction.staticFriction;
        if (ft2 > fn2)
        {
            newF_t = F_t * fn2 / (ft2+1.0e-20);
            nOverlap_t = -newF_t / kt; //slippage has occurred so the tangential overlap is reduced a bit
            
            //at this point we get energy loss from the sliding!                                                           
            F_td = newF_t;
        }
        else
        {
            //at this point we get energy loss from the damping!
            F_td = -relVel_t * 2.0 * dampingRatio * sqrt(kt * nEquivMass);
            newF_t = F_t + F_td;
        }

        // Fill in parameters we were passed in
        contactResults.tangentialForce.setX(newF_t.getX());
        contactResults.tangentialForce.setY(newF_t.getY());
        contactResults.tangentialForce.setZ(newF_t.getZ());

        contactResults.usTangentialForce.setX(F_td.getX());
        contactResults.usTangentialForce.setY(F_td.getY());
        contactResults.usTangentialForce.setZ(F_td.getZ());

        tangentialPhysicalOverlap.setX(nOverlap_t.getX());
        tangentialPhysicalOverlap.setY(nOverlap_t.getY());
        tangentialPhysicalOverlap.setZ(nOverlap_t.getZ());

    }

    // questions: in this stage, do we only have normal spring force? as particles are not in physical contact (contact radius=0 while (alpha_c-alpha_c0<0. Example: speical case alpha_c0=0))
    else if (contact.normalPhysicalOverlap < alpha_c0 && customResults[1] == 1.0) 
    {    
        /*----------------- Calculation of linear cohesive Model ---------------  */
        if (nSurfaceEnergy > 0.0)
        {
            double alpha_fp = alpha_p - fcp / ke - 4.0 / 9.0 * fcp / (ke / kekcRatio);

            if (contact.normalPhysicalOverlap > alpha_cp)
            {
                normalForceMag = normalForceElastic;
                kn = ke;
            }
            else
            {
                normalForceMag = normalForceAdhesive;
                kn = ke/kekcRatio;
            }

            // questions: no damping force as particles not in real contact and then no wave disspation through contact?

            if (contact.normalPhysicalOverlap < alpha_fp)
            {
                normalForceMag = 0.0;
                customResultsDelta[1] = 0.0 - customResults[1];
            }


            F_n = -unitCPVect * normalForceMag;

            contactResults.normalForce.setX(F_n.getX());
            contactResults.normalForce.setY(F_n.getY());
            contactResults.normalForce.setZ(F_n.getZ());
        }

    }

    customResultsDelta[3] = normalForceMag - customResults[3]; // update normalForceMag

    return eSuccess;
}

unsigned int CElastoPlasticAdhesive_V3_2_0::getPartPartContactParameterData(const char elem1Type[], const char elem2Type[], void* parameterData)
{
    CElastoPlasticAdhesiveParameters* params = reinterpret_cast<CElastoPlasticAdhesiveParameters*>(parameterData);
    *params = m_ElastoPlasticAdhesiveParameters.getElastoPlasticAdhesiveParameters(elem1Type, elem2Type);
    return sizeof(CElastoPlasticAdhesiveParameters);
}

unsigned int CElastoPlasticAdhesive_V3_2_0::getPartGeomContactParameterData(const char elem1Type[], const char elem2Type[], void* parameterData)
{
    CElastoPlasticAdhesiveParameters* params = reinterpret_cast<CElastoPlasticAdhesiveParameters*>(parameterData);
    *params = m_ElastoPlasticAdhesiveParameters.getElastoPlasticAdhesiveParameters(elem1Type, elem2Type);
    return sizeof(CElastoPlasticAdhesiveParameters);
}

unsigned int CElastoPlasticAdhesive_V3_2_0::getNumberOfRequiredProperties(const EPluginPropertyCategory category)
{
    //register 1 particle custom property
    if (eContact == category)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

bool CElastoPlasticAdhesive_V3_2_0::getDetailsForProperty(unsigned int                    propertyIndex,
                                   NApi::EPluginPropertyCategory   category,
                                   char                            name[NApi::CUSTOM_PROP_MAX_NAME_LENGTH],
                                   NApi::EPluginPropertyDataTypes& dataType,
                                   unsigned int&                   numberOfElements,
                                   NApi::EPluginPropertyUnitTypes& unitType,
                                   char                            initValBuff[NApi::BUFF_SIZE])
{
    size_t bufferLimit = NApi::CUSTOM_PROP_MAX_NAME_LENGTH;
    // Define the characteristic of the properties: name, number of elements, unit
    if(0 == propertyIndex && eContact == category)
    {
        strncpy(name, ContactElastoPlasticAdhesiveResults.c_str(), bufferLimit);
        dataType = eDouble;
        numberOfElements = 4;
        unitType = eNone;
        return true;
    }
    else
    {
        return false;
    }
}