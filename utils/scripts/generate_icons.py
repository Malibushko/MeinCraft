#
# Config
#

BASE_DIR           = "../../"
BLOCK_CONFIGS_PATH = BASE_DIR + "res/configs/blocks_uv.json"
OUTPUT_PATH        = BASE_DIR + "res/icons/"
ICON_SIZE          = (64, 64)
WINDOW_SIZE        = (512, 512)

#
# Code
#

import glfw
from OpenGL.GL import *
from OpenGL.GL.shaders import compileProgram, compileShader
import numpy as np
import pyrr
import json
from PIL import Image

vertex_src = """
# version 330
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_color;
layout(location = 2) in vec2 a_texture;
uniform mat4 rotation;
out vec3 v_color;
out vec2 v_texture;
void main()
{
    gl_Position = rotation * vec4(a_position, 1.0);
    v_color = a_color;
    v_texture = a_texture;
}
"""

fragment_src = """
# version 330
in vec3 v_color;
in vec2 v_texture;
out vec4 out_color;
uniform sampler2D s_texture;
void main()
{
    out_color = texture(s_texture, v_texture); //* vec4(v_color, 1.0f);
}
"""

# glfw callback functions
def window_resize(window, width, height):
    glViewport(0, 0, width, height)

def save_image(path, width, height):
    # Read the pixels from the OpenGL frame buffer
    pixels = glReadPixels(0, 0, WINDOW_SIZE[0], WINDOW_SIZE[1], GL_RGBA, GL_UNSIGNED_BYTE)

    # Convert the pixel data to a PIL image
    image = Image.frombytes("RGBA", (WINDOW_SIZE[0], WINDOW_SIZE[1]), pixels)

    # Flip the image vertically (OpenGL's origin is the lower-left corner)
    image = image.transpose(Image.FLIP_TOP_BOTTOM)
    image = image.resize((width, height), resample=Image.Resampling.NEAREST)
    
    # Save the image to a file
    image.save(path)

def get_uv_for_block_face(UVData, atlas_size, face):
    UVMin = np.array(UVData[face])
    UVMax = UVMin + np.array(quad_size)
    
    UVMin = UVMin / np.array(atlas_size)
    UVMax = UVMax / np.array(atlas_size)
    
    return UVMin, UVMax

def generate_block_data(UVData, atlas_size):
    FrontMin, FrontMax   = get_uv_for_block_face(UVData, atlas_size, "front")
    BackMin,  BackMax    = get_uv_for_block_face(UVData, atlas_size, "back")
    LeftMin,  LeftMax    = get_uv_for_block_face(UVData, atlas_size, "left")
    RightMin, RightMax   = get_uv_for_block_face(UVData, atlas_size, "right")
    TopMin,   TopMax     = get_uv_for_block_face(UVData, atlas_size, "top")
    BottomMin, BottomMax = get_uv_for_block_face(UVData, atlas_size, "bottom")
    
    return np.array([
            # front
            -0.5, -0.5,  -0.5,  1.0, 0.0, 0.0,  FrontMax[0], FrontMax[1],
             -0.5, 0.5,  -0.5,  0.0, 1.0, 0.0,  FrontMax[0], FrontMin[1],
             0.5,  0.5,  -0.5,  0.0, 0.0, 1.0,  FrontMin[0], FrontMin[1],
            0.5,  -0.5,  -0.5,  1.0, 1.0, 1.0,  FrontMin[0], FrontMax[1],
            # back
            -0.5, -0.5, 0.5,  1.0, 0.0, 0.0,  BackMin[0], BackMax[1],
             0.5, -0.5, 0.5,  0.0, 1.0, 0.0,  BackMax[0], BackMax[1],
             0.5,  0.5, 0.5,  0.0, 0.0, 1.0,  BackMax[0], BackMin[1],
            -0.5,  0.5, 0.5,  1.0, 1.0, 1.0,  BackMin[0], BackMin[1],
            #left
             -0.5, -0.5, -0.5,  1.0, 0.0, 0.0,  LeftMax[0], LeftMax[1],
             -0.5,  0.5, -0.5,  0.0, 1.0, 0.0,  LeftMax[0], LeftMin[1],
             -0.5,  0.5,  0.5,  0.0, 0.0, 1.0,  LeftMin[0], LeftMin[1],
             -0.5, -0.5,  0.5,  1.0, 1.0, 1.0,  LeftMin[0], LeftMax[1],
            #right
            0.5,  -0.5, -0.5,  1.0, 0.0, 0.0,  RightMax[0], RightMax[1],
            0.5, -0.5, 0.5,  0.0, 1.0, 0.0,    RightMin[0], RightMax[1],
            0.5, 0.5,  0.5,  0.0, 0.0, 1.0,    RightMin[0], RightMin[1],
            0.5,  0.5,  -0.5,  1.0, 1.0, 1.0,  RightMax[0], RightMin[1],
            #top
            -0.5, 0.5, 0.5,  1.0, 0.0, 0.0,    TopMax[0], TopMin[1],
             0.5, 0.5, 0.5,  0.0, 1.0, 0.0,    TopMin[0], TopMin[1],
             0.5, 0.5,  -0.5,  0.0, 0.0, 1.0,  TopMin[0], TopMax[1],
            -0.5, 0.5,  -0.5,  1.0, 1.0, 1.0,  TopMax[0], TopMax[1],
            #bottom
             -0.5,  -0.5, 0.5,  1.0, 0.0, 0.0,  BottomMax[0], BottomMin[1],
            -0.5, -0.5, -0.5,  0.0, 1.0, 0.0,   BottomMin[0], BottomMin[1],
             0.5, -0.5,  -0.5,  0.0, 0.0, 1.0,  BottomMin[0], BottomMax[1],
             0.5,  -0.5,  0.5,  1.0, 1.0, 1.0,  BottomMax[0], BottomMax[1],
    ], dtype=np.float32)
          
def create_block_vao(UVData, atlas_size):
    vertices = generate_block_data(UVData, atlas_size)

    indices = [0,  1,  2,  2,  3,  0,
            4,  5,  6,  6,  7,  4,
            8,  9, 10, 10, 11,  8,
            12, 13, 14, 14, 15, 12,
            16, 17, 18, 18, 19, 16,
            20, 21, 22, 22, 23, 20]

    vertices = np.array(vertices, dtype=np.float32)
    indices = np.array(indices, dtype=np.uint32)

    # Vertex Buffer Object
    VBO = glGenBuffers(1)
    glBindBuffer(GL_ARRAY_BUFFER, VBO)
    glBufferData(GL_ARRAY_BUFFER, vertices.nbytes, vertices, GL_STATIC_DRAW)

    # Element Buffer Object
    EBO = glGenBuffers(1)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO)
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.nbytes, indices, GL_STATIC_DRAW)

    glEnableVertexAttribArray(0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertices.itemsize * 8, ctypes.c_void_p(0))

    glEnableVertexAttribArray(1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertices.itemsize * 8, ctypes.c_void_p(12))

    glEnableVertexAttribArray(2)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, vertices.itemsize * 8, ctypes.c_void_p(24))
   
# initializing glfw library
if not glfw.init():
    raise Exception("glfw can not be initialized!")

# creating the window
window = glfw.create_window(WINDOW_SIZE[0], WINDOW_SIZE[1], "My OpenGL window", None, None)

# check if window was created
if not window:
    glfw.terminate()
    raise Exception("glfw window can not be created!")

# set the callback function for window resize
glfw.set_window_size_callback(window, window_resize)

# make the context current
glfw.make_context_current(window)

shader = compileProgram(compileShader(vertex_src, GL_VERTEX_SHADER), compileShader(fragment_src, GL_FRAGMENT_SHADER))

f = open(BLOCK_CONFIGS_PATH, "r")
blocks_uv_config = json.loads(f.read())

atlas_path = BASE_DIR + blocks_uv_config["atlas"]
atlas_size = blocks_uv_config["atlas_size"]
quad_size  = blocks_uv_config["quad_size"]

# load image
image = Image.open(atlas_path)
img_data = image.convert("RGBA").tobytes()

glUseProgram(shader)
glClearColor(0, 0.1, 0.1, 0.0)
glEnable(GL_DEPTH_TEST)
glEnable(GL_BLEND)
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)

rotation_loc = glGetUniformLocation(shader, "rotation")

texture = glGenTextures(1)
glBindTexture(GL_TEXTURE_2D, texture)
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img_data)

# Set the texture wrapping parameters
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT)
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT)
# Set texture filtering parameters
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST)
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST)

for block in blocks_uv_config["blocks"]:
    icon_out_path = OUTPUT_PATH + str(block["id"]) + ".png"
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    glClearColor(0, 0.1, 0.1, 0.0)

    if len(block["uv"]) == 0:
        continue
    
    create_block_vao(block["uv"], atlas_size)

    rot_x = pyrr.Matrix44.from_x_rotation(0.5)
    rot_y = pyrr.Matrix44.from_y_rotation(0.5)

    glUniformMatrix4fv(rotation_loc, 1, GL_FALSE, pyrr.matrix44.multiply(rot_x, rot_y))

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, None)
    
    save_image(icon_out_path, ICON_SIZE[0], ICON_SIZE[1])
    print(f"Saved to {icon_out_path}")
    
    glfw.swap_buffers(window)
    glfw.poll_events()
    
# terminate glfw, free up allocated resources
glfw.terminate()