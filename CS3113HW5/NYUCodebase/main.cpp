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

class Vector3 {
public:
    Vector3(){};
    
    float length() const
    {
        return sqrt((x*x)+(y*y));
    }
    
    Vector3 operator *(const Matrix &a)
    {
        
        
        float b=a.m[0][0]*x+a.m[1][0]*y+a.m[2][0]*z+a.m[3][0];
        float c=a.m[0][1]*x+a.m[1][1]*y+a.m[2][1]*z+a.m[3][1];
        float d=a.m[0][2]*x+a.m[1][2]*y+a.m[2][2]*z+a.m[3][2];
        return Vector3(b,c,d);
    }
    
    
    Vector3(float x, float y, float z):x(x), y(y), z(z){
        
    }
    float x;
    float y;
    float z;
    
    
};

bool testSATSeparationForEdge(float edgeX, float edgeY, const std::vector<Vector3> &points1, const std::vector<Vector3> &points2, Vector3 &penetration) {
    float normalX = -edgeY;
    float normalY = edgeX;
    float len = sqrtf(normalX*normalX + normalY*normalY);
    normalX /= len;
    normalY /= len;
    
    std::vector<float> e1Projected;
    std::vector<float> e2Projected;
    
    for(int i=0; i < points1.size(); i++) {
        e1Projected.push_back(points1[i].x * normalX + points1[i].y * normalY);
    }
    for(int i=0; i < points2.size(); i++) {
        e2Projected.push_back(points2[i].x * normalX + points2[i].y * normalY);
    }
    
    std::sort(e1Projected.begin(), e1Projected.end());
    std::sort(e2Projected.begin(), e2Projected.end());
    
    float e1Min = e1Projected[0];
    float e1Max = e1Projected[e1Projected.size()-1];
    float e2Min = e2Projected[0];
    float e2Max = e2Projected[e2Projected.size()-1];
    
    float e1Width = fabs(e1Max-e1Min);
    float e2Width = fabs(e2Max-e2Min);
    float e1Center = e1Min + (e1Width/2.0);
    float e2Center = e2Min + (e2Width/2.0);
    float dist = fabs(e1Center-e2Center);
    float p = dist - ((e1Width+e2Width)/2.0);
    
    if(p >= 0) {
        return false;
    }
    
    float penetrationMin1 = e1Max - e2Min;
    float penetrationMin2 = e2Max - e1Min;
    
    float penetrationAmount = penetrationMin1;
    if(penetrationMin2 < penetrationAmount) {
        penetrationAmount = penetrationMin2;
    }
    
    penetration.x = normalX * penetrationAmount;
    penetration.y = normalY * penetrationAmount;
    
    return true;
}

bool penetrationSort(const Vector3 &p1, const Vector3 &p2) {
    return p1.length() < p2.length();
}

bool checkSATCollision(const std::vector<Vector3> &e1Points, const std::vector<Vector3> &e2Points, Vector3 &penetration) {
    std::vector<Vector3> penetrations;
    if(e1Points.size()==0||e2Points.size()==0)
    {
        return false;
    }
    for(int i=0; i < e1Points.size(); i++) {
        float edgeX, edgeY;
        
        if(i == e1Points.size()-1) {
            edgeX = e1Points[0].x - e1Points[i].x;
            edgeY = e1Points[0].y - e1Points[i].y;
        } else {
            edgeX = e1Points[i+1].x - e1Points[i].x;
            edgeY = e1Points[i+1].y - e1Points[i].y;
        }
        Vector3 penetration;
        bool result = testSATSeparationForEdge(edgeX, edgeY, e1Points, e2Points, penetration);
        if(!result) {
            return false;
        }
        penetrations.push_back(penetration);
    }
    for(int i=0; i < e2Points.size(); i++) {
        float edgeX, edgeY;
        
        if(i == e2Points.size()-1) {
            edgeX = e2Points[0].x - e2Points[i].x;
            edgeY = e2Points[0].y - e2Points[i].y;
        } else {
            edgeX = e2Points[i+1].x - e2Points[i].x;
            edgeY = e2Points[i+1].y - e2Points[i].y;
        }
        Vector3 penetration;
        bool result = testSATSeparationForEdge(edgeX, edgeY, e1Points, e2Points, penetration);
        
        if(!result) {
            return false;
        }
        penetrations.push_back(penetration);
    }
    
    std::sort(penetrations.begin(), penetrations.end(), penetrationSort);
    penetration = penetrations[0];
    
    Vector3 e1Center;
    for(int i=0; i < e1Points.size(); i++) {
        e1Center.x += e1Points[i].x;
        e1Center.y += e1Points[i].y;
    }
    e1Center.x /= (float)e1Points.size();
    e1Center.y /= (float)e1Points.size();
    
    Vector3 e2Center;
    for(int i=0; i < e2Points.size(); i++) {
        e2Center.x += e2Points[i].x;
        e2Center.y += e2Points[i].y;
    }
    e2Center.x /= (float)e2Points.size();
    e2Center.y /= (float)e2Points.size();
    
    Vector3 ba;
    ba.x = e1Center.x - e2Center.x;
    ba.y = e1Center.y - e2Center.y;
    
    if( (penetration.x * ba.x) + (penetration.y * ba.y) < 0.0f) {
        penetration.x *= -1.0f;
        penetration.y *= -1.0f;
    }
    
    return true;
}

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
        acceleration=Vector3(0, 0, 0);
    }
    
    Vector3 position;
    Vector3 velocity;
    Vector3 acceleration;
    Vector3 size;
    float rotation;
    vector<Vector3>worldCoords;
    
    void Draw(ShaderProgram& program)
    {
        Matrix modelMatrix;
        modelMatrix.identity();
        //modelMatrix.Rotate(3.1416/5*angle);
        modelMatrix.Translate(position.x, position.y, position.z);
        modelMatrix.Rotate(rotation);
        modelMatrix.Scale(size.x, size.y, size.z);
        
        worldCoords.clear();
        worldCoords.push_back( Vector3(-0.5,-0.5,0)*modelMatrix);
        worldCoords.push_back(Vector3(-0.5,0.5,0)*modelMatrix);
        worldCoords.push_back(Vector3(0.5,0.5,0)*modelMatrix);
        worldCoords.push_back(Vector3(0.5,-0.5,0)*modelMatrix);
        
        
        
        
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

/*bool checkCollision(Entity& bullet, Entity& enemy){
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
    
}*/
/*
bool checkWin(vector<Entity> enemyVec)
{
    for(int i=0;i<enemyVec.size();i++)
    {
        if(enemyVec[i].isVisible)
        {
            return false;
        }
    }
    return true;
}*/



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
    GLuint shooterTexture=LoadTexture(RESOURCE_FOLDER"cyanWall.png");
    GLuint font=LoadTexture(RESOURCE_FOLDER"font2.png");
    
    Entity shooter=Entity("shooter", shooterTexture,Vector3(0, 1, 0),Vector3(-1, 0, 0), Vector3(1, 1, 1),true,true);
    
    
    
    Entity shooter2=Entity("shooter", shooterTexture,Vector3(3, 1, 0),Vector3(1, 0, 0), Vector3(1, 1, 1),true,true);
    
    
    Entity shooter3=Entity("shooter", shooterTexture,Vector3(-2, 0, 0),Vector3(0.5, 0.5, 0), Vector3(1, 1, 1),true,true);
    
    shooter.rotation=1;
    shooter2.rotation=-1;
    shooter3.rotation=2;
    
    
    
    //vector<Entity> enemyVec;
    //vector<Entity>blockVec;
   // vector<Entity>bullets;
    
   // int curBulletIndex=0;
    
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
        
            
            
        
        /*else if(keys[SDL_SCANCODE_SPACE]){
            
            shooter.position.y+= shooter.velocity.y * elapsed;
            
        }*/
            
            
            
            Vector3 p=Vector3(0,0,0);
            if(checkSATCollision(shooter.worldCoords, shooter2.worldCoords, p))
            {
                shooter.position.x+=p.x/2*1.01;
                shooter.position.y+=p.y/2*1.01;
                //shooter.position.z+=p.z/2*1.01;
                
                shooter2.position.x-=p.x/2*1.01;
                shooter2.position.y-=p.y/2*1.01;
                //shooter2.position.z-=p.z/2*1.01;
            }
            if(checkSATCollision(shooter.worldCoords, shooter3.worldCoords, p))
            {
                shooter.position.x+=p.x/2*1.01;
                shooter.position.y+=p.y/2*1.01;
                //shooter.position.z+=p.z/2*1.01;
                
                shooter3.position.x-=p.x/2*1.01;
                shooter3.position.y-=p.y/2*1.01;
                //shooter3.position.z-=p.z/2*1.01;
            }
            if(checkSATCollision(shooter3.worldCoords, shooter2.worldCoords, p))
            {
                shooter3.position.x+=p.x/2*1.01;
                shooter3.position.y+=p.y/2*1.01;
                //shooter3.position.z+=p.z/2*1.01;
                
                shooter2.position.x-=p.x/2*1.01;
                shooter2.position.y-=p.y/2*1.01;
                //shooter2.position.z-=p.z/2*1.01;
            }
            
        
            shooter.velocity.y+=shooter.acceleration.y*elapsed;
            shooter.position.y+=shooter.velocity.y*elapsed;
            shooter.velocity.x+=shooter.acceleration.x*elapsed;
            shooter.position.x+=shooter.velocity.x*elapsed;
            
            shooter2.velocity.y+=shooter2.acceleration.y*elapsed;
            shooter2.position.y+=shooter2.velocity.y*elapsed;
            shooter2.velocity.x+=shooter2.acceleration.x*elapsed;
            shooter2.position.x+=shooter2.velocity.x*elapsed;
            
            shooter3.velocity.y+=shooter3.acceleration.y*elapsed;
            shooter3.position.y+=shooter3.velocity.y*elapsed;
            shooter3.velocity.x+=shooter3.acceleration.x*elapsed;
            shooter3.position.x+=shooter3.velocity.x*elapsed;
            
            shooter.Draw(program);
            shooter2.Draw(program);
            shooter3.Draw(program);
            
            
            
            
        
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


