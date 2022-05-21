#if !defined CElastoPlasticAdhesive_V3_2_0_h
#define CElastoPlasticAdhesive_V3_2_0_h


#include "IPluginContactModelV3_2_0.h"
#include "CParametersList.h"

/**
 * This class provides an implementation of IPluginContactModelV3_2_0
 * That logs calls to standard error, run edem with --noFree on windows
 * to see these
 */
class CElastoPlasticAdhesive_V3_2_0 : public NApiCm::IPluginContactModelV3_2_0
{
public:

    /**
     * Constructor, does nothing
     */
    CElastoPlasticAdhesive_V3_2_0() = default;

    /**
     * Destructor, does nothing
     */
    ~CElastoPlasticAdhesive_V3_2_0() = default;

    /**
     * Sets prefFileName to the empty string as this plugin has no
     * configuration
     *
     * Implementation of method from IPluginContactModelV3_2_0
     */
    void getPreferenceFileName(char prefFileName[NApi::FILE_PATH_MAX_LENGTH]) override;

    /**
     * Returns true to indicate plugin is thread safe
     *
     * Implementation of method from IPluginContactModelV3_2_0
     */
    bool isThreadSafe() override;

    /**
     * Returns true to indicate the plugin uses custom properties
     *
     * Implementation of method from IPluginContactModelV3_2_0
     */
    bool usesCustomProperties() override;

    /**
     * How the model should interact with chaining
     */
    NApi::EPluginModelType getModelType() override;
    NApi::EPluginExecutionChainPosition getExecutionChainPosition() override;

    /**
    * Gets the file name of the gpu plugin
    *
    * Implementation of method from IPluginContactModelV3_2_0
    */
    void getGpuFileName(char gpuNameFile[NApi::FILE_PATH_MAX_LENGTH]) override;

    /**
     * Does nothing
     *
     * Implementation of method from IPluginContactModelV3_2_0
     */

    virtual void setFilePath(const char simFile[]);

    bool setup(NApiCore::IApiManager_1_0& apiManager,
        const char                 prefFile[],
        char* customMsg) override;

    /**
     * Executes before the simulation starts. Gets the property
     * index of each custom property.
     *
     * Implementation of method from IPluginContactModelV3_2_0
     */
    bool starting(NApiCore::IApiManager_1_0& apiManager, int numThreads) override;

    /**
    * Does nothing
    *
    * Implementation of method from IPluginContactModelV3_2_0
    */
    void stopping(NApiCore::IApiManager_1_0& apiManager) override;

     /**
     * Calculates forces when two elements are in contact
     *
     * Implementation of method from IPluginContactModelV3_2_0
     */
    NApi::ECalculateResult calculateForce(int threadID,
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
        NCalcForceTypesV3_0_0::SContactResult& contactResults) override;

    /**
     * Returns 1 for eSimulation, otherwise 0
     *
     * Implementation of method from IPluginContactModelV3_2_0
     */
    unsigned int getNumberOfRequiredProperties(const NApi::EPluginPropertyCategory category) override;

    /**
     * Returns info when eSimulation so that configForTimeStep() is called.
     *
     * Implementation of method from IPluginContactModelV3_2_0
     */
    bool getDetailsForProperty(unsigned int propertyIndex,
        NApi::EPluginPropertyCategory category,
        char name[NApi::CUSTOM_PROP_MAX_NAME_LENGTH],
        NApi::EPluginPropertyDataTypes& dataType,
        unsigned int& numberOfElements,
        NApi::EPluginPropertyUnitTypes& unitType,
        char initValBuff[NApi::BUFF_SIZE]);


    unsigned int getPartPartContactParameterData(const char elem1Type[], const char elem2Type[], void* parameterData) override;
    unsigned int getPartGeomContactParameterData(const char elem1Type[], const char elem2Type[], void* parameterData) override;


    /* user-defined varaibles in cpp file */
    unsigned int m_ContactElastoPlasticAdhesiveResults_index;

private:
    std::string simulationPath;
    CParametersList m_ElastoPlasticAdhesiveParameters;

};

#endif
