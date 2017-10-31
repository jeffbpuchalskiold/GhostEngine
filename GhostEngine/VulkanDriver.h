#include "GhostEngine.h"
#include "Render.h"
#include <cstring>

using namespace std;

#pragma warning (disable : 4267)

class VulkanDriver
{
public:
    Render* render;
    //Create window
    GLFWwindow* window;

    void run()
    {
        initVulkan();
        mainLoop();
    }
#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif
        


//------------------------------------------- Public Variables ------------------------------------------------------------------------------

    //----------------------------------------------- End of Variables----------------------------------------------------------------------
private:
    void initVulkan()
    {
        render = new Render();
        createInstance();		
        setupDebugCallback();
        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();
        createSwapChain();
        createImageViews();
        createGraphicsPipeline();
        
    }

    void mainLoop()
    {

    }

//----------------------------------------- VK DRIVERS USER METHODS ------------------------------------------------------------------------
    /*
    * Validate if system is able to handle Layer suport
    */
    bool vKValidateLayerSupport()
    {


        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> avaliableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, avaliableLayers.data());

        for (const char* layerName : validationLayers)
        {
            bool layerFound = false;

            for (const auto& layerProperties : avaliableLayers)
            {
                if (strcmp(layerName, layerProperties.layerName) == 0)
                {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound)
            {
                return false;
            }
        }

        return true;
    }
    /*
    * Create a Requeried extesnion function to enable essentials extrension.
    */
    std::vector<const char*> getRequiredExtensions()
    {
        std::vector<const char*> extensions;

        unsigned int glfwExtensionCount = 0;
        const char** glfwExtensions;

        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        for (unsigned int i = 0; i < glfwExtensionCount; i++)
        {
            extensions.push_back(glfwExtensions[i]);
        }

        if (enableValidationLayers)
        {
            extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
        }
        
        return extensions;
    }
    /*
    * Create a Wrapper For vulkanCallback.
    * @param VkDebugReportFlagsEXT flags = bit flag for debug report.
    * ex: VK_DEBUG_REPORT_INFORMATION_BIT_EXT,
    *	  VK_DEBUG_REPORT_WARNING_BIT_EXT,
    *	  VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT,
    *	  VK_DEBUG_REPORT_ERROR_BIT_EXT,
    *	  VK_DEBUG_REPORT_DEBUG_BIT_EX
    * @param VkDebugReportObjectTypeEXT = Object type for message.
    *  ex: VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_EXT is for VkPhysicalDevice.
    * @param uint64_t obj = vulkan callback need.
    * @param size_t location = vulkan callback need.
    * @param int32_t code = vulkan callback need.
    * @param const char* layerPrefix = vulkan callback need.
    * @param const char* msg = A pointer to Message.
    * @param void* userData = A potiner to hold user content data.
    * @returns VK_FAILURE = a default return for this function.
    */

    /* Setup debugCallback */
    void setupDebugCallback()
    {
        //See if we need validation layer 
        if (!enableValidationLayers) return;

        //Setup Debug Report
        VkDebugReportCallbackCreateInfoEXT createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
        createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
        createInfo.pfnCallback = debugCallback;

        
        if (CreateDebugReportCallbackEXT(instance, &createInfo, nullptr, &callback) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to set up debug callback!");
        }
    }
    
    /*
    /* Create a instance to VulkanDriver
    */
    void createInstance()
    {
        window = glfwCreateWindow(800, 600, "Ghost Engine Vulkan 1.0 V", nullptr, nullptr);
        // See if we got a validation layer
        if (enableValidationLayers && !vKValidateLayerSupport())
        {
            throw std::runtime_error("we need validation layer, but is not available!");
        }

        //Struct for Vulkan app info
        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Ghost Engine";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "GhostEngine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;


    


        //Create A Struct with CreateInfo
        VkInstanceCreateInfo createInfo = {};

        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

        // get layer extensions 
        auto extensionsz = getRequiredExtensions();
        createInfo.ppEnabledExtensionNames = extensionsz.data();
        createInfo.enabledExtensionCount = extensionsz.size();
    

        //See if we got layers 
        if (enableValidationLayers)
        {
            createInfo.enabledLayerCount = validationLayers.size();
            createInfo.ppEnabledLayerNames = validationLayers.data();
        }
        else
        {
            createInfo.enabledLayerCount = 0;
        }

        if (vkCreateInstance(&createInfo, nullptr, instance.replace()) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create a instance!");
        }

        //uint32_t extensionCount = 0;
        //vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        //std::vector<VkExtensionProperties> extensions(extensionCount);

        std::cout << "Avaliable extensions:" << std::endl;
        for (const auto& extension : extensionsz)
        {
            std::cout << extension << std::endl;
        }



    }

    void pickPhysicalDevice()
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
        std::vector<VkPhysicalDevice> devices(deviceCount); 
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        if (deviceCount == 0) 
        {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }

        std::multimap<int, VkPhysicalDevice> candidates;
        

        for (const auto& device : devices)
        {
            int score = rateDevicesSuitability(device);
            candidates.insert(std::make_pair(score, device));

            if(candidates.rbegin()->first > 0) 
            {
                physicalDevice = candidates.rbegin()->second;
            }
            else
            {
                throw std::runtime_error("failed to find a suitable GPU!");
            }

            if (score < 1000)
            {
                SHOWMESSAGE(L"Error We cant get a video card with suported caracteristics",
                            L"GPU Error", MB_ICONERROR);
            }
            if (isDeviceSuitable(device))
            {
                physicalDevice = device;
                break;
            }
        }		
        
    }

    int rateDevicesSuitability(VkPhysicalDevice device)
    {
        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
        int score = 0;

        //Discrete GPUS have more advantage in selection
        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            score += 1000;
        }

        // Maximum possible size of textures affects graphics quality
        score += deviceProperties.limits.maxImageDimension2D;

        std::cout << "Device Name: " << deviceProperties.deviceName << std::endl;

        //Application cannot work without shaders.
        if (!deviceFeatures.geometryShader)
        {
#ifdef _WIN32
            
            MessageBox(NULL, L"Your GPU does not have suport for shaders.", L"GPU Shader not Found.", 0);
#endif
        }

        return score;
    }

    bool isDeviceSuitable(VkPhysicalDevice device)
    {
        bool swapChainAdequate = false;
        QueueFamilyIndices indices = findQueueFamilies(device);
        
        bool extensionSupported = checkDeviceExtensionSupport(device);
        if (extensionSupported)
        {
            SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }

        return indices.isComplete() && extensionSupported && swapChainAdequate;
    }
    
    bool checkDeviceExtensionSupport(VkPhysicalDevice device)
    {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
        
        std::vector<VkExtensionProperties> avaliableExtension(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, avaliableExtension.data());
        std::set<std::string> requireExtension(deviceExtensions.begin(), deviceExtensions.end());
        
        for (const auto& extension : avaliableExtension)
        {
            requireExtension.erase(extension.extensionName);
        }

        return requireExtension.empty();
    }
    
    void createLogicalDevice()
    {
        QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<int> uniqueQueueFamilies = { indices.graphicsFamily, indices.presentFamily };

        float queuePriority = 1.0f;

        for (int queueFamily : uniqueQueueFamilies)
        {
            VkDeviceQueueCreateInfo queueCreateInfo = {};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;

            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);

        }

        VkPhysicalDeviceFeatures deviceFeatures = {};
        VkDeviceCreateInfo createInfo = {};

        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.queueCreateInfoCount = (uint32_t)queueCreateInfos.size();

        createInfo.pEnabledFeatures = &deviceFeatures;
        
        createInfo.enabledExtensionCount = deviceExtensions.size();
        createInfo.ppEnabledExtensionNames= deviceExtensions.data();

        if (enableValidationLayers)
        {
            createInfo.enabledLayerCount = validationLayers.size();
            createInfo.ppEnabledLayerNames = validationLayers.data();
        }
        else
        {
            createInfo.enabledLayerCount = 0;
        }

        if (vkCreateDevice(physicalDevice, &createInfo, nullptr, device.replace()) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create logical device!");
#ifdef DEBUGUI
            SHOWMESSAGE(L"Failed to create logical device\nAborting.", L"GPU Error", MB_ICONASTERISK);
#endif
        }

        vkGetDeviceQueue(device, indices.graphicsFamily, 0, &presentQueue);

    }
    
    void createSurface()
    {
        if(glfwCreateWindowSurface(instance, window, nullptr, surface.replace()) != VK_SUCCESS)
        {
            std::runtime_error("Failed to create a window surface");
        }
    }
    
    void createSwapChain()
    {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
        {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR  createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface;

        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
        uint32_t queueFamilyIndices[] = { (uint32_t)indices.graphicsFamily,(uint32_t)indices.presentFamily };
        
        if (indices.graphicsFamily != indices.presentFamily)
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0; //Optional
            createInfo.pQueueFamilyIndices = nullptr; //Optional
        }

        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(device, &createInfo, nullptr, swapChain.replace()) != VK_SUCCESS)
        {
            runtime_error("failed to create swap chain!");
        }

        vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());
        
        swapChainImageFormat = surfaceFormat.format;
        swapChainExtent = extent;
    }

    void createImageViews()
    {
        swapChainImageViews.resize(swapChainImages.size(), VDeleter<VkImageView>{device, vkDestroyImageView});
        for (int32_t i = 0; i < swapChainImages.size(); i++)
        {
            VkImageViewCreateInfo createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            createInfo.image = swapChainImages[i];

            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = swapChainImageFormat;

            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(device, &createInfo, nullptr, swapChainImageViews[i].replace()) != VK_SUCCESS)
            {
                SHOWMESSAGE(L"Failed to create image views", L"Vulkan Driver Error.", MB_ICONERROR | MB_OK);
                throw runtime_error("Failed to create image views!");
            }
        }

    }

    void createGraphicsPipeline()
    {
        createShaderModule(render->readFile("vert.spv"), vertShaderModule );
        createShaderModule(render->readFile("frag.spv"), vertShaderModule );

        VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;

        vertShaderStageInfo.module = vertShaderModule;
        vertShaderStageInfo.pName = "main";
    }
        

    void createShaderModule(const std::vector<char>& code, VDeleter<VkShaderModule>& shaderModule)
    {
        VkShaderModuleCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();

        std::vector<uint32_t> codeAligned(code.size() / sizeof(uint32_t) + 1);
        memcpy(codeAligned.data(), code.data() , code.size());

        createInfo.pCode = codeAligned.data();
        if (vkCreateShaderModule(device, &createInfo, nullptr, shaderModule.replace()) != VK_SUCCESS)
        {
            ERRORMESSAGE(L"Failied to create ShaderModule.\nPlease verify your code.", L"Error in ShaderModule");
            throw std::runtime_error("Failed to create Shader Module!");
        }
    }

    
//----------------------------------- END OF METHODS ----------------------------------------------------------------------------------


//--------------------------------- VK SPECIFIC METHODS--------------------------------------------------------------------------------
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t obj, size_t location, int32_t code, const char* layerPrefix, const char* msg, void* userData)
    {
        std::cerr << "validation layer: " << msg << std::endl;

        return VK_FALSE;
    }
    
    VkResult CreateDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackCreateInfoEXT*pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback)
    {
        auto func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");

        if (func != nullptr)
        {
            return func(instance, pCreateInfo, pAllocator, pCallback);
        }
        else
        {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }


    }

    void DestroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator)
    {
        auto func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
        if (func != nullptr)
        {
            func(instance, callback, pAllocator);
        }
    }


    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& avaliableFormats)
    {
        if (avaliableFormats.size() == 1 && avaliableFormats[0].format == VK_FORMAT_UNDEFINED)
        {
            return{ VK_FORMAT_B8G8R8A8_UNORM, VK_COLORSPACE_SRGB_NONLINEAR_KHR };
        }

        for (const auto& avaliableFormat : avaliableFormats)
        {
            if(avaliableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && avaliableFormat.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
            {
                return avaliableFormat;
            }
        }

        return avaliableFormats[0];
    }

    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> avaliblePresentModes)
    {
        VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

        for (const auto& avaliblePresentMode : avaliblePresentModes)
        {
            if (avaliblePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                return avaliblePresentMode;
            }
            else if(avaliblePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
            {
                bestMode = avaliblePresentMode;
            }
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
    {
        if (capabilities.currentExtent.width != (std::numeric_limits<uint32_t>::max)())
        {
            return capabilities.currentExtent;
        }
        else
        {
            VkExtent2D actualExtent = { 1080,1920 };


            actualExtent.width = max(capabilities.minImageExtent.width, min(capabilities.maxImageExtent.width, actualExtent.width));
            actualExtent.height = max(capabilities.minImageExtent.height, min(capabilities.maxImageExtent.height, actualExtent.height));

            return actualExtent;
        }
    }
//------------------------------------------- END OF METHODS --------------------------------------------------------------------------------



// Create Layer Validation
    const std::vector<const char*> validationLayers = {
        "VK_LAYER_LUNARG_standard_validation" };
    //Deleter function
    VDeleter<VkInstance> instance{ vkDestroyInstance };
    //Handle for debug callbakc.
    VkDebugReportCallbackEXT callback;
    //Handle to Device;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

    //Logical device = Nedd clean with VDeleter.
    VDeleter<VkDevice> device{ vkDestroyDevice };
    VDeleter<VkSurfaceKHR> surface{ instance , vkDestroySurfaceKHR };
    VDeleter<VkSwapchainKHR> swapChain{ device , vkDestroySwapchainKHR };
    VDeleter<VkShaderModule> vertShaderModule{ device, vkDestroyShaderModule };
    VDeleter<VkShaderModule> fragShaderModule{ device, vkDestroyShaderModule };

    //Struct to get Families and set it
    struct QueueFamilyIndices
    {
        int graphicsFamily = -1;
        int presentFamily = -1;

        bool isComplete()
        {
            return graphicsFamily >= 0 && presentFamily >= 0; 
        }
    };
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device)
    {
        

        QueueFamilyIndices indices;
        uint32_t queueFamilyCount = 0;

        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
    
        //interete to find familes
        int i = 0;
        
        for (const auto& queueFamily : queueFamilies)
        {
            

            if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                indices.graphicsFamily = i;

                VkBool32 presentSupport = false;
                vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
                if (queueFamily.queueCount > 0 && presentSupport)
                {
                    indices.presentFamily = i;
                }
                
            }

            

            if (indices.isComplete())
            {
                break;
            }
        }	

        
        return indices;

    }

    //Struct to SwapChain
    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device)
    {
        SwapChainSupportDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
        if (formatCount != 0)
        {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

        if (presentModeCount != 0)
        {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
        }

        return details;
    }

    //Parameters
    VkQueue graphicsQueue;	
    VkQueue presentQueue;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;

    //Vectors 
    std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    std::vector<VkImage> swapChainImages;
    std::vector<VDeleter<VkImageView>> swapChainImageViews;

    
};
