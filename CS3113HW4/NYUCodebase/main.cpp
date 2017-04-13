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
#include<vector>


using namespace std;

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif


SDL_Window* displayWindow;
Matrix viewMatrix;



class SheetSprite {
public:
    SheetSprite();
    SheetSprite(unsigned int textureID, float u, float v, float width, float height, float
                size);
    
    void Draw(ShaderProgram *program);
    
    float size;
    unsigned int textureID;
    float u;
    float v;
    float width;
    float height;
    
};



void SheetSprite::Draw(ShaderProgram *program) {
    glBindTexture(GL_TEXTURE_2D, textureID);
    GLfloat texCoords[] = {
        u, v+height,
        u+width, v,
        u, v,
        u+width, v,
        u, v+height,
        u+width, v+height
    };
    float aspect = width / height;
    float vertices[] = {
        -0.5f * size * aspect, -0.5f * size,
        0.5f * size * aspect, 0.5f * size,
        -0.5f * size * aspect, 0.5f * size,
        0.5f * size * aspect, 0.5f * size,
        -0.5f * size * aspect, -0.5f * size ,
        0.5f * size * aspect, -0.5f * size};
    // draw our arrays
}
/*
void Render() {
    enemySprite.Draw(program);
}
 */

class Vector3 {
public:
    Vector3(){};
    
    Vector3(float x, float y, float z):x(x), y(y), z(z){
    
    }
    float x;
    float y;
    float z;
    
};

void DrawText(ShaderProgram *program, int fontTexture, string text, float size, float spacing) {
    float texture_size = 1.0/16.0f;
    vector<float> vertexData;
    vector<float> texCoordData;
    
    for(int i=0; i < text.size(); i++) {
        int spriteIndex = (int)text[i];
        float texture_x = (float)(spriteIndex % 16) / 16.0f;
        float texture_y = (float)(spriteIndex / 16) / 16.0f;
        
        vertexData.insert(vertexData.end(), {
            ((size+spacing) * i) + (-0.5f * size), 0.5f * size,
            ((size+spacing) * i) + (-0.5f * size), -0.5f * size,
            ((size+spacing) * i) + (0.5f * size), 0.5f * size,
            ((size+spacing) * i) + (0.5f * size), -0.5f * size,
            ((size+spacing) * i) + (0.5f * size), 0.5f * size,
            ((size+spacing) * i) + (-0.5f * size), -0.5f * size,
        });
        texCoordData.insert(texCoordData.end(), {
            texture_x, texture_y,
            texture_x, texture_y + texture_size,
            texture_x + texture_size, texture_y,
            texture_x + texture_size, texture_y + texture_size,
            texture_x + texture_size, texture_y,
            texture_x, texture_y + texture_size,
        });
    }
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
    glEnableVertexAttribArray(program->positionAttribute);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    
    glBindTexture(GL_TEXTURE_2D, fontTexture);
    glDrawArrays(GL_TRIANGLES, 0, text.size() *6);
    
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
    // draw this data (use the .data() method of std::vector to get pointer to data)
}

class Entity {
public:
    Entity();
    
    Entity(string typ, GLuint tex, Vector3 pos, Vector3 vel, Vector3 siz, bool lef, bool vis)
    {
        textureID=tex;
        position=pos;
        velocity=vel;
        size=siz;
        type=typ;
        left=lef;
        isVisible=vis;
        acceleration=Vector3(0, -2, 0);
    }
    
    Vector3 position;
    Vector3 velocity;
    Vector3 acceleration;
    Vector3 size;
    
    void Draw(ShaderProgram& program)
    {
        Matrix modelMatrix;
        modelMatrix.identity();
        //modelMatrix.Rotate(3.1416/5*angle);
        modelMatrix.Translate(position.x, position.y, position.z);
        program.setModelMatrix(modelMatrix);
        
        
        glBindTexture(GL_TEXTURE_2D, textureID);
        
        float vertices[] ={-0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5};
        float vertices2[]={-0.3, -0.3, 0.3, -0.3, 0.3, 0.3, -0.3, -0.3, 0.3, 0.3, -0.3, 0.3};
        float vertices3[]={-0.1, -0.1, 0.1, -0.1, 0.1, 0.1, -0.1, -0.1, 0.1, 0.1, -0.1, 0.1};
        
        float texCoords[] = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
        float texCoords1[]={0.0,0.25,0.125,0.25,0.125,0.0,0.0,0.25,0.125,0.0,0.0,0.0};
        float texCoords2[]={0,1.0,0.125,1.0,0.125,0.75,0,1.0,0.125,0.75,0,0.75};
        
        if(type=="shooter")
        {
            width=0.5;
            height=0.5;
            glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
            glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords1);
        }
        else if (type=="block")
        {
            width=0.5;
            height=0.5;
            glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
            glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords1);
        }
        else
        {
            width=0.1;
            height=0.1;
            glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices3);
            glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
        }
        
        glEnableVertexAttribArray(program.positionAttribute);
     //   glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
        glEnableVertexAttribArray(program.texCoordAttribute);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    float x;
    float y;
    bool isVisible;
    GLuint textureID;
    string type;
    
    float width;
    float height;
    float speed;
    float direction_x;
    float direction_y;
    bool left;
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

bool checkCollision(Entity& bullet, Entity& enemy){
    if(!bullet.isVisible)
        return false;
    else if(!enemy.isVisible)
        return false;
    if((enemy.position.x+enemy.width/2>bullet.position.x-bullet.width/2&&enemy.position.x-enemy.width/2<bullet.position.x+bullet.width/2))
    {
        
        if((enemy.position.y+enemy.height/2>bullet.position.y-bullet.height/2&&enemy.position.y-enemy.height/2<bullet.position.y+bullet.height/2))
        {
            bullet.isVisible=false;
            enemy.isVisible=false;
            return true;
        }
    }
    return false;
    
}

bool checkWIn(vector<Entity> enemyVec)
{
    for(int i=0;i<enemyVec.size();i++)
    {
        if(enemyVec[i].isVisible)
        {
            return false;
        }
    }
    return true;
}



enum GameState { MENU, PLAY, STATE_GAME_OVER};

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
    GLuint blockTexture= LoadTexture(RESOURCE_FOLDER"cyanWall.png");
    GLuint shooterTexture=LoadTexture(RESOURCE_FOLDER"spriteSheet.png");
    GLuint font=LoadTexture(RESOURCE_FOLDER"font2.png");
    
    Entity shooter=Entity("shooter", shooterTexture,Vector3(0, 1, 0),Vector3(1, 1, 0), Vector3(1, 1, 1),true,true);
    shooter.Draw(program);
    vector<Entity> enemyVec;
    vector<Entity>blockVec;
    vector<Entity>bullets;
    
    int curBulletIndex=0;
    
    int x=-20.0;
    for(int i=0;i<40;i++)
    {
        Entity block=Entity("block", blockTexture,Vector3(x, -1.2, 0),Vector3(0, 0, 0), Vector3(1, 1, 1),true,true);
        blockVec.push_back(block);
        x+=1.0;
    }
    Entity block=Entity("block", blockTexture,Vector3(3, 0.2, 0),Vector3(0, 0, 0), Vector3(1, 1, 1),true,true);
    enemyVec.push_back(block);
    /*
    for(int i=0;i<5;i++)
    {
        Entity newBullet=Entity("bullet", snowTexture,Vector3(shooter.position.x, shooter.position.y, 0),Vector3(0, 0, 0), Vector3(1, 1, 1),true,false);
        
        bullets.push_back(newBullet);
    }*/
    
    GameState state=MENU;
    Matrix projectionMatrix;
    Matrix modelMatrix;
    //Matrix viewMatrix;
    
    
    
    projectionMatrix.setOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);
    
    glUseProgram(program.programID);
    SDL_Event event;
    
    float lastFrameTicks = 0.0f;


    
    bool done = false;
    bool gameOver=false;
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    
    while (!done) {
        float ticks = (float)SDL_GetTicks()/1000.0f;
        float elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        
        
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
            else if(state==MENU&&event.type == SDL_SCANCODE_S)
            {
                state=PLAY;
                
            }
            else if(state==PLAY&&event.type == SDL_KEYDOWN) {
                if(event.key.keysym.scancode==SDL_SCANCODE_SPACE)
                {
                    
                    shooter.velocity.y=2;
                    
                }
                
            }
            
        }
        
        
        
        glClearColor(0.0f, 0.4f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        
        viewMatrix.identity();
        viewMatrix.Translate(-shooter.position.x, 0, 0);
    

        
        
        
        program.setModelMatrix(modelMatrix);
        program.setProjectionMatrix(projectionMatrix);
        program.setViewMatrix(viewMatrix);
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        if(state==MENU)
        {
            DrawText(&program,font
                     , "S to start", 0.25, 0);
            if (keys[SDL_SCANCODE_S]) {
                state=PLAY;         }
        }
        
        else if(state==PLAY)
        {
        
        
        
        if (keys[SDL_SCANCODE_LEFT]) {
            if (shooter.position.x - shooter.size.x / 2 > -3.55f*3)
                shooter.position.x-= shooter.velocity.x * elapsed;
        }
        else if (keys[SDL_SCANCODE_RIGHT]) {
            if (shooter.position.x + shooter.size.x / 2< 3.55f*3)
                shooter.position.x+= shooter.velocity.x * elapsed;
        }
            shooter.velocity.y+=shooter.acceleration.y*elapsed;
            shooter.position.y+=shooter.velocity.y*elapsed;
            
            if((shooter.position.y-shooter.size.y/2)<(-1.2+block.size.y/2))
            {
                //shooter.velocity.y=0;
                if(shooter.velocity.y<=0)
                {
                    shooter.position.y=(shooter.size.y/2-1.2+block.size.y/2);
                }
            }
            
        
        /*else if(keys[SDL_SCANCODE_SPACE]){
            
            shooter.position.y+= shooter.velocity.y * elapsed;
            
        }*/
        

        
        
        
        shooter.Draw(program);
            
            
            
            if(gameOver)
            {
                DrawText(&program, font, "You Lost!!", 0.25, 0.1);
                for(int i=0;i<enemyVec.size();i++)
                {
                    enemyVec[i].isVisible=false;
                }
            }
            for(int i=0;i<blockVec.size();i++)
            {
                blockVec[i].Draw(program);
            }
        for(int i=0;i<enemyVec.size();i++)
        {
            
            
 
                if(checkCollision(enemyVec[i], shooter))
                {
                    
                    gameOver=true;
                    
                }
            if(enemyVec[i].isVisible)
                enemyVec[i].Draw(program);
        }
            
        
        
        //enemy.Draw(program);
        //glDisableVertexAttribArray(program.positionAttribute);
        // glDisableVertexAttribArray(program.texCoordAttribute);
        }
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


