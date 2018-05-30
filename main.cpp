#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include <math.h>
#include <direct.h>  

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>
#include <cstdio>
#include <iostream>
#include <png.h>
#include <string>

#include <fstream>
#include <iomanip>
#include <sstream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
bool save_png_libpng(const char *filename, uint8_t *pixels, int w, int h);

// settings
const unsigned int SCR_WIDTH = 640;
const unsigned int SCR_HEIGHT = 480;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main(int argc, char *argv[])
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

														 // glfw window creation
														 // --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile shaders
	// -------------------------
	Shader ourShader("1.model_loading.vs", "1.model_loading.fs");

	// load models
	// -----------
	Model ourModel(argv[1]);

	// camera position
	vector<glm::vec3> camera_positions;
	float phi = (sqrt(5) - 1) / 2;
	camera_positions.push_back(glm::vec3(0.0f, 1.0f, phi));
	camera_positions.push_back(glm::vec3(0.0f, 1.0f, -phi));
	camera_positions.push_back(glm::vec3(0.0f, -1.0f, phi));
	camera_positions.push_back(glm::vec3(0.0f, -1.0f, -phi));
	camera_positions.push_back(glm::vec3(1.0f, phi, 0));
	camera_positions.push_back(glm::vec3(-1.0f, phi, 0));
	camera_positions.push_back(glm::vec3(1.0f, -phi, 0));
	camera_positions.push_back(glm::vec3(-1.0f, -phi, 0));
	camera_positions.push_back(glm::vec3(phi, 0, 1));
	camera_positions.push_back(glm::vec3(phi, 0, -1));
	camera_positions.push_back(glm::vec3(-phi, 0, 1));
	camera_positions.push_back(glm::vec3(-phi, 0, -1));

	glm::vec3 Position = glm::vec3(0.0f,0.0f,3.0f);

	int counterloop = 0;

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// don't forget to enable shader before setting uniforms
		ourShader.use();

		// view/projection transformations
		//generate projection
		float alpha = 585.0f;
		float belta = 585.0f;
		float cx = 320;
		float cy = 240;
		float n = 0.1f;
		float f = 100.0f;
		float projection_array[16] = {
			alpha / cx, 0.0f, 0.0f, 0.0f,
			0.0f, belta / cy, 0.0f, 0.0f,
			0.0f, 0.0f, -(f + n) / (f - n),-2 * f*n / (f - n),
			0.0f, 0.0f, -1.0f, 0.0f
		};
		glm::mat4 my_projection;
		memcpy(glm::value_ptr(my_projection), projection_array, sizeof(projection_array));
		my_projection = glm::transpose(my_projection);

		/*glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);*/
		if (counterloop < 12) {
			Position = camera_positions[counterloop]*30.0f;
		}
		
		glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 Origin = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::mat4 view = glm::lookAt(Position, Origin, Up); 

		//view = camera.GetViewMatrix();
		glm::mat4 view_transpose = glm::transpose(view);
		glm::mat4 view_inverse = glm::inverse(view);
		glm::mat4 view_transpose_inverse = glm::inverse(view_transpose);
		ourShader.setMat4("projection", my_projection);
		ourShader.setMat4("view", view);

		// render the loaded model
		glm::mat4 model;
		// model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
		model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));	// it's a bit too big for our scene, so scale it down
		ourShader.setMat4("model", model);
		ourModel.Draw(ourShader);

		// save image
		if (counterloop <12) {
			GLubyte* pixels = new GLubyte[3 * SCR_WIDTH * SCR_HEIGHT];

			glReadPixels(0, 0, SCR_WIDTH, SCR_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, pixels);

			//Set the file name to save
			std::string data_path = argv[1];
			size_t pos_slash = data_path.find_last_of("/\\");
			string filename = data_path.substr(pos_slash + 1);
			size_t pos_dot = filename.find_last_of(".");
			string data_ID = filename.substr(0, pos_dot);
			string dirpath = ".\\" + data_ID;
			_mkdir(dirpath.c_str());
			ostringstream curr_frame_prefix;
			curr_frame_prefix << std::setw(6) << std::setfill('0') << counterloop;
			string image_file = ".\\"+ data_ID + "\\frame-" + curr_frame_prefix.str() + ".depth.png";
			string base2world_file = ".\\" + data_ID + "\\frame-" + curr_frame_prefix.str() + ".pose.txt";

			// save position
			ofstream out(base2world_file);
			if (out.is_open()) {
				out << scientific;
				out << view_transpose_inverse[0][0] << " ";
				out << view_transpose_inverse[0][1] << " ";
				out << view_transpose_inverse[0][2] << " ";
				out << view_transpose_inverse[0][3];
				out << endl;
				out << view_transpose_inverse[1][0] << " ";
				out << view_transpose_inverse[1][1] << " ";
				out << view_transpose_inverse[1][2] << " ";
				out << view_transpose_inverse[1][3];
				out << endl;
				out << view_transpose_inverse[2][0] << " ";
				out << view_transpose_inverse[2][1] << " ";
				out << view_transpose_inverse[2][2] << " ";
				out << view_transpose_inverse[2][3];
				out << endl;
				out << view_transpose_inverse[3][0] << " ";
				out << view_transpose_inverse[3][1] << " ";
				out << view_transpose_inverse[3][2] << " ";
				out << view_transpose_inverse[3][3];
			}


			// save image
			save_png_libpng(image_file.c_str(), pixels, SCR_WIDTH, SCR_HEIGHT);
		}
		counterloop++;
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}


bool save_png_libpng(const char *filename, uint8_t *pixels, int w, int h)
{
	png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	if (!png)
		return false;

	png_infop info = png_create_info_struct(png);
	if (!info) {
		png_destroy_write_struct(&png, &info);
		return false;
	}

	FILE *fp = fopen(filename, "wb");
	if (!fp) {
		png_destroy_write_struct(&png, &info);
		return false;
	}

	png_init_io(png, fp);
	png_set_IHDR(png, info, w, h, 8 /* depth */, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	png_colorp palette = (png_colorp)png_malloc(png, PNG_MAX_PALETTE_LENGTH * sizeof(png_color));
	if (!palette) {
		fclose(fp);
		png_destroy_write_struct(&png, &info);
		return false;
	}
	png_set_PLTE(png, info, palette, PNG_MAX_PALETTE_LENGTH);
	png_write_info(png, info);
	png_set_packing(png);

	png_bytepp rows = (png_bytepp)png_malloc(png, h * sizeof(png_bytep));
	for (int i = 0; i < h; ++i)
		rows[i] = (png_bytep)(pixels + (h - i -1) * w * 3);

	png_write_image(png, rows);
	png_write_end(png, info);
	png_free(png, palette);
	png_destroy_write_struct(&png, &info);

	fclose(fp);
	delete[] rows;
	return true;
}