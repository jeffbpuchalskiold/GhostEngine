#include "GhostEngine.h"
#include "VulkanDriver.h"






//Main entry point.
int main()
{
	VulkanDriver driver;
	
	// init glfw
	glfwInit();
	//Set glfw Window hint
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	glm::mat4 matrix;
	glm::vec4 vec;

	auto test = matrix * vec;

	// Enable RUN
	try
	{
		driver.run();

	}
	catch (const std::runtime_error& e)
	{
		std::cerr << e.what() << std::endl;

		PRINT("This is Error %s", e.what());
		system("pause");
		return EXIT_FAILURE;
	}

	while(!glfwWindowShouldClose(driver.window))
	{
		glfwPollEvents();
	}

	glfwDestroyWindow(driver.window);
	glfwTerminate();

	
	return EXIT_SUCCESS;


}