#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"
#include "Matrix.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif


SDL_Window* displayWindow;

class Entity {
public:
    void Draw();
    float x;
    float y;
    float rotation;
    int textureID;
    
    float width;
    float height;
    float speed;
    float direction_x;
    float direction_y;
};


GLuint LoadTexture(const char *filePath) {
    int w,h,comp;
    unsigned char* image = stbi_load(filePath, &w, &h, &comp, STBI_rgb_alpha);
    
    if(image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }
    
    GLuint retTexture;
    glGenTextures(1, &retTexture);
    glBindTexture(GL_TEXTURE_2D, retTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(image);
    return retTexture;
}

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 360);
    
    ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
    
    GLuint snowTexture = LoadTexture(RESOURCE_FOLDER"frozenOrb.png");
    GLuint paddleTexture=LoadTexture(RESOURCE_FOLDER"i-brick.png");
    GLuint p1WinTexture=LoadTexture(RESOURCE_FOLDER"player1Win.png");
    GLuint p2WinTexture=LoadTexture(RESOURCE_FOLDER"player2Win.png");
    
    
    Matrix projectionMatrix;
    Matrix modelMatrix;
    Matrix viewMatrix;
    
    
    projectionMatrix.setOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);
    
    glUseProgram(program.programID);
    SDL_Event event;
    
    float lastFrameTicks = 0.0f;
    float angle=3.1416*2;
    bool left=false;
    bool up=true;
    
    bool done = false;
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    float p1Pos=0;
    float p2Pos=0;
    float ballxPos=0;
    float ballyPos=0;
    bool gameOver=false;
    
    while (!done) {
        float ticks = (float)SDL_GetTicks()/1000.0f;
        float elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        
        
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
            else if(event.type == SDL_KEYDOWN) {
                if(event.key.keysym.scancode==SDL_SCANCODE_UP)
                {
                    p1Pos+=25*elapsed;
                }
                if(event.key.keysym.scancode==SDL_SCANCODE_DOWN)
                {
                    p1Pos-=25*elapsed;
                }
                if(event.key.keysym.scancode==SDL_SCANCODE_Q)
                {
                    p2Pos+=25*elapsed;
                }
                if(event.key.keysym.scancode==SDL_SCANCODE_A)
                {
                    p2Pos-=25*elapsed;
                }
                
                // event.jbutton.which tells us which controller (e.g. 0,1,etc.)
                // event.jbutton.button tells us which button was pressed (0,1,2…etc)
            }
            
            
        
        }
        
        
        
        glClearColor(0.0f, 0.4f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        
        if(ballyPos>2-0.4)
        {
            up=false;
        }
        else if(ballyPos<(-2+0.4))
        {
            up=true;
        }
        
        if(ballxPos>3.15)
        {
            if((p1Pos+.4+0.2)>ballyPos && (p1Pos-0.4-0.2)<ballyPos&&!gameOver)
                left=true;
            else
            {
                gameOver=true;
                modelMatrix.identity();
                program.setModelMatrix(modelMatrix);
                program.setProjectionMatrix(projectionMatrix);
                program.setViewMatrix(viewMatrix);
                
                
                glBindTexture(GL_TEXTURE_2D, p2WinTexture);
                
                float vertices[] ={-1, -1, 1, -1, 1, 1, -1, -1, 1, 1, -1, 1};
                glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
                glEnableVertexAttribArray(program.positionAttribute);
                
                float texCoords[] = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
                glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
                glEnableVertexAttribArray(program.texCoordAttribute);
                
                glDrawArrays(GL_TRIANGLES, 0, 6);

            }
        }
        else if(ballxPos<-3.15)
        {
            if((p2Pos+0.4+0.2)>ballyPos && (p2Pos-0.4-0.2)<ballyPos&&!gameOver)
                left=false;
            else
            {
                gameOver=true;
                modelMatrix.identity();
                program.setModelMatrix(modelMatrix);
                program.setProjectionMatrix(projectionMatrix);
                program.setViewMatrix(viewMatrix);
                
                
                glBindTexture(GL_TEXTURE_2D, p1WinTexture);
                
                float vertices[] ={-1, -1, 1, -1, 1, 1, -1, -1, 1, 1, -1, 1};
                glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
                glEnableVertexAttribArray(program.positionAttribute);
                
                float texCoords[] = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
                glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
                glEnableVertexAttribArray(program.texCoordAttribute);
                
                glDrawArrays(GL_TRIANGLES, 0, 6);
                
            }
        }
        
        if(left)
        {
            ballxPos-=2*elapsed;
        }
        else
        {
            ballxPos+=2*elapsed;
        }
        if(up)
        {
            ballyPos+=1*elapsed;
            
        }
        else
        {
            ballyPos-=1*elapsed;
        }
        
        
        
        
        modelMatrix.identity();
        //modelMatrix.Rotate(3.1416/5*angle);
        modelMatrix.Translate(ballxPos, ballyPos, 0);
        program.setModelMatrix(modelMatrix);
        program.setProjectionMatrix(projectionMatrix);
        program.setViewMatrix(viewMatrix);
        
        
        glBindTexture(GL_TEXTURE_2D, snowTexture);
        
        float vertices[] ={-0.2, -0.2, 0.2, -0.2, 0.2, 0.2, -0.2, -0.2, 0.2, 0.2, -0.2, 0.2};
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
        glEnableVertexAttribArray(program.positionAttribute);
        
        float texCoords[] = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
        glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
        glEnableVertexAttribArray(program.texCoordAttribute);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
      //
      
        //
        
        glBindTexture(GL_TEXTURE_2D, paddleTexture);
        
        
        
        float vertices2[] = {-0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5};

        
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices2);
        glEnableVertexAttribArray(program.positionAttribute);
        
        float texCoords2[] = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
        glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords2);
        glEnableVertexAttribArray(program.texCoordAttribute);
        
      //  modelMatrix.Translate(0, 0, 0);
        modelMatrix.identity();
        modelMatrix.Translate(3.4, 0, 0 );
        if(p1Pos>2-0.4)
        {
            p1Pos=2-0.4;
        }
        else if(p1Pos<(-2+0.4))
        {
            p1Pos=-2+0.4;
        }
        modelMatrix.Translate(0, p1Pos, 0);
        
        
        program.setModelMatrix(modelMatrix);
        program.setProjectionMatrix(projectionMatrix);
        program.setViewMatrix(viewMatrix);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        glBindTexture(GL_TEXTURE_2D, paddleTexture);
        
        
        
        float vertices3[] = {-0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5};
        
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices3);
        glEnableVertexAttribArray(program.positionAttribute);
        
        float texCoords3[] = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
        glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords3);
        glEnableVertexAttribArray(program.texCoordAttribute);
        
        //  modelMatrix.Translate(0, 0, 0);
        modelMatrix.identity();
        modelMatrix.Translate(-3.4, 0, 0 );
        if(p2Pos>2-0.4)
        {
            p2Pos=2-0.4;
        }
        else if(p2Pos<(-2+0.4))
        {
            p2Pos=-2+0.4;
        }
        modelMatrix.Translate(0, p2Pos, 0);
        
        program.setModelMatrix(modelMatrix);
        program.setProjectionMatrix(projectionMatrix);
        program.setViewMatrix(viewMatrix);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        //glDisableVertexAttribArray(program.positionAttribute);
        // glDisableVertexAttribArray(program.texCoordAttribute);
        
        SDL_GL_SwapWindow(displayWindow);
    }
    /*
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 5, 5, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, "colorDrift.png");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    
    */
    SDL_Quit();
    return 0;
}


