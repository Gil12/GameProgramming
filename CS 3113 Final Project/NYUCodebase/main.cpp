#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"
#include "Matrix.h"

#include "SDL_mixer.h"

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

/*

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
        float vertices4[]={-0.15, -0.15, 0.15, -0.15, 0.15, 0.15, -0.15, -0.15, 0.15, 0.15, -0.15, 0.15};
        float vertices5[]={-2, -2, 2, -2, 2, 2, -2, -2, 2, 2, -2, 2};
        float vertices6[]={-1, -1, 1, -1, 1, 1, -1, -1, 1, 1, -1, 1};

        
        
        float texCoords[] = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
        
        if(type=="player")
        {
            width=0.5;
            height=0.5;
            glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices2);
            glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
        }
        else if (type=="ball")
        {
            width=0.3;
            height=0.3;
            glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices4);
            glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
        }
        else if(type=="block")
        {
            width=0.3;
            height=0.3;
            glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices4);
            glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
        }
        else if(type=="bar")
        {
            width=2;
            height=2;
            glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices5);
            glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
        }
        else if(type=="text")
        {
            width=1;
            height=1;
            glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices6);
            glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
        }
        else{
            width=1;
            height=1;
            glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
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
void update(Entity &left, Entity &right){
    right.rotation+=0.05;
    left.rotation-=0.05;
    
    
    double xVal=3.125-0.2+.3;
    double yVal=1.575-0.2+.3;
    if(left.position.x>-left.width/2)
    {
        left.position.x=-left.width/2;

    }
    if(left.position.x<-xVal)
    {
        left.position.x=-xVal;
    
    }
    //1.85-0.15/2-0.6/2=1.575
    if(left.position.y<-yVal)
    {
        left.position.y=-yVal;

    }
    if(left.position.y>yVal)
    {
        left.position.y=yVal;

    }
    if(right.position.x>xVal)
    {
        right.position.x=xVal;

    }
    if(right.position.x<right.width/2)
    {
        right.position.x=right.width/2;

    }
    if(right.position.y<-yVal)
    {
        right.position.y=-yVal;
    }
    if(right.position.y>yVal)
    {
        right.position.y=yVal;
    }

}

float absolute(float num)
{
    if (num<0)
    {
        num*=-1;
    }
    return num;
}

void updateBalls(vector<Entity> &ball, float elapsed)
{
    
    for(int i=0;i<ball.size();i++)
    {
        ball[i].position.x+= ball[i].velocity.x * elapsed;
        ball[i].position.y+= ball[i].velocity.y * elapsed;        //3.4-.15/2-0.6/2=3.125
        double xVal=3.125+.3;
        double yVal=1.575+.3;
        if(ball[i].position.x>xVal)
        {
            ball[i].position.x=xVal;
            if(ball[i].velocity.x>0)
            {
                ball[i].velocity.x*=-1;
            }
        }
        if(ball[i].position.x<-xVal)
        {
            ball[i].position.x=-xVal;
            if(ball[i].velocity.x<0)
            {
                ball[i].velocity.x*=-1;
            }
        }
        //1.85-0.15/2-0.6/2=1.575
        if(ball[i].position.y<-yVal)
        {
            ball[i].position.y=-yVal;
            if(ball[i].velocity.y<0)
            {
                ball[i].velocity.y*=-1;
            }
        }
        if(ball[i].position.y>yVal)
        {
            ball[i].position.y=yVal;
            if(ball[i].velocity.y>0)
            {
                ball[i].velocity.y*=-1;
            }
        }
    }
}

int collide(Entity cue, Entity &ball, int color,Mix_Chunk *cueSound)
{
    //int radius;
    if(sqrt((ball.position.x-cue.position.x)*(ball.position.x-cue.position.x)+(ball.position.y-cue.position.y)*(ball.position.y-cue.position.y))<ball.width/2+cue.width/2)
    {
        Mix_PlayChannel( -1, cueSound, 0);
        
        if(cue.left&&ball.left)
        {
            ball.velocity.x=5;
            ball.velocity.y=1.5*(ball.position.y-cue.position.y)/sqrt((ball.position.x-cue.position.x)*(ball.position.x-cue.position.x)+(ball.position.y-cue.position.y)*(ball.position.y-cue.position.y));
            if(color==2)
            {
                ball.velocity.x=7;
                ball.velocity.y=2*(ball.position.y-cue.position.y)/sqrt((ball.position.x-cue.position.x)*(ball.position.x-cue.position.x)+(ball.position.y-cue.position.y)*(ball.position.y-cue.position.y));
            }
        }
        else if(!cue.left&&!ball.left)
        {
            ball.velocity.x=-5;
            ball.velocity.y=1.5*(ball.position.y-cue.position.y)/sqrt((ball.position.x-cue.position.x)*(ball.position.x-cue.position.x)+(ball.position.y-cue.position.y)*(ball.position.y-cue.position.y));
            if(color==2)
            {
                ball.velocity.x=-7;
                ball.velocity.y=2*(ball.position.y-cue.position.y)/sqrt((ball.position.x-cue.position.x)*(ball.position.x-cue.position.x)+(ball.position.y-cue.position.y)*(ball.position.y-cue.position.y));
            }
        }
        else
        {
            if(cue.left&&!ball.left)
            {
                
                return 1;
            }
            else if(!cue.left&&ball.left)
            {
                return 2;
            }
        }
        
    }
    return 0;
}



bool collideBalls(Entity &ball1, Entity &ball2,GLuint light)
{
    if(sqrt((ball1.position.x-ball2.position.x)*(ball1.position.x-ball2.position.x)+(ball1.position.y-ball2.position.y)*(ball1.position.y-ball2.position.y))<ball1.width/2+ball2.width/2)
    {
        float b1mass=1;
        float b2mass=1;
        if(ball1.textureID==light)
        {
            b1mass=0.5;
        }
        if(ball2.textureID==light)
        {
            b2mass=0.5;
        }
        float velX1=(ball1.velocity.x * (b1mass - b2mass) + (2 * b2mass * ball2.velocity.x)) / (b1mass + b2mass);
        float velY1=(ball1.velocity.y * (b1mass - b2mass) + (2 * b2mass * ball2.velocity.y)) / (b1mass + b2mass);
        float velX2=(ball2.velocity.x * (b2mass - b1mass) + (2 * b1mass * ball1.velocity.x)) / (b1mass + b2mass);
        float velY2=(ball2.velocity.y * (b2mass - b1mass) + (2 * b1mass * ball1.velocity.y)) / (b1mass + b2mass);
       /* float velX1 = ( *(2 * 0.5 * ball2.velocity.x)) / (1);
        float velY1 = ( (2 * 0.5 * ball2.velocity.y)) / (1);
        float velX2 = ( (2 *0.5 * ball1.velocity.x)) / (1);
        float velY2 = ( (2 * 0.5 * ball1.velocity.y)) / (1);*/
        if(velX1==0)
        {
            velX1+=velX2*0.25;
            velX2-=velX2*0.25;
        }
        if(velX2==0)
        {
            velX2+=velX1*0.25;
            velX1-=velX1*0.25;
        }
        if(velY2==0)
        {
            velY2+=velY1*0.25;
            velY1-=velY1*0.25;
        }
        if(velY1==0)
        {
            velY1+=velY2*0.25;
            velY2-=velY2*0.25;
        }
        
        ball1.position.x +=velX1*.03;
        ball1.position.y += velY1*.03;
        ball2.position.x +=  velX2*.03;
        ball2.position.y +=velY2*.03;
        
        ball1.velocity.x=velX1;
        ball1.velocity.y=velY1;
        ball2.velocity.x=velX2;
        ball2.velocity.y=velY2;
        return true;
    }
    return false;
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
    
    GLuint airBallTexture = LoadTexture(RESOURCE_FOLDER"airBall.png");
    GLuint iceBallTexture= LoadTexture(RESOURCE_FOLDER"iceBall.png");
    GLuint fireBallTexture= LoadTexture(RESOURCE_FOLDER"fireBall.png");

    GLuint lightningBallTexture= LoadTexture(RESOURCE_FOLDER"lightningBall.png");

    
    GLuint airCueTexture = LoadTexture(RESOURCE_FOLDER"airCue.png");
    GLuint iceCueTexture= LoadTexture(RESOURCE_FOLDER"iceCue.png");
    GLuint fireCueTexture= LoadTexture(RESOURCE_FOLDER"fireCue.png");
    GLuint lightningCueTexture= LoadTexture(RESOURCE_FOLDER"lightningCue.png");
    GLuint blockTexture= LoadTexture(RESOURCE_FOLDER"cyanWall.png");
    GLuint barTexture= LoadTexture(RESOURCE_FOLDER"barVertical.png");
    GLuint texture0= LoadTexture(RESOURCE_FOLDER"0.png");
    GLuint texture1= LoadTexture(RESOURCE_FOLDER"1.png");
    GLuint texture2= LoadTexture(RESOURCE_FOLDER"2.png");
    GLuint texture3= LoadTexture(RESOURCE_FOLDER"3.png");
    //GLuint texture4= LoadTexture(RESOURCE_FOLDER"4.png");
    //GLuint texture5= LoadTexture(RESOURCE_FOLDER"5.png");
    GLuint flameTexture=LoadTexture(RESOURCE_FOLDER"flame.png");
   // GLuint backgroundTex=LoadTexture(RESOURCE_FOLDER"backGround.png");
    //Entity background=Entity("bar", backgroundTex,Vector3(0, 0, 0),Vector3(0, 0, 0), Vector3(1, 1, 1),true,true);
    vector<GLuint> cueTexture;
    vector<GLuint> ballTexture;
    cueTexture.push_back(airCueTexture);
    cueTexture.push_back(iceCueTexture);
    cueTexture.push_back(fireCueTexture);
    cueTexture.push_back(lightningCueTexture);
    
    ballTexture.push_back(airBallTexture);
    ballTexture.push_back(iceBallTexture);
    ballTexture.push_back(fireBallTexture);
    ballTexture.push_back(lightningBallTexture);
    
    

    GLuint winTexture= LoadTexture(RESOURCE_FOLDER"winnerSquare.png");
    
    vector<Entity> leftScoreVec;
    vector<Entity> rightScoreVec;
    Entity l0=Entity("player", texture0,Vector3(-0.5, 1.2, 0),Vector3(0, 0, 0), Vector3(1, 1, 1),true,true);
    Entity l1=Entity("player", texture1,Vector3(-0.5, 1.2, 0),Vector3(0, 0, 0), Vector3(1, 1, 1),true,true);
    Entity l2=Entity("player", texture2,Vector3(-0.5, 1.2, 0),Vector3(0, 0, 0), Vector3(1, 1, 1),true,true);
    Entity l3=Entity("player", texture3,Vector3(-0.5, 1.2, 0),Vector3(0, 0, 0), Vector3(1, 1, 1),true,true);
    
    
    Entity r0=Entity("player", texture0,Vector3(0.5, 1.2, 0),Vector3(0, 0, 0), Vector3(1, 1, 1),true,true);
    Entity r1=Entity("player", texture1,Vector3(0.5, 1.2, 0),Vector3(0, 0, 0), Vector3(1, 1, 1),true,true);
    Entity r2=Entity("player", texture2,Vector3(0.5, 1.2, 0),Vector3(0, 0, 0), Vector3(1, 1, 1),true,true);
    Entity r3=Entity("player", texture3,Vector3(0.5, 1.2, 0),Vector3(0, 0, 0), Vector3(1, 1, 1),true,true);
    
    
    
    
    GLuint font=LoadTexture(RESOURCE_FOLDER"font2.png");
    //1.2
    Entity playerRight=Entity("player", iceCueTexture,Vector3(3, 0, 0),Vector3(2, 2, 0), Vector3(1, 1, 1),false,true);
    //playerRight.Draw(program);
    Entity playerLeft=Entity("player", airCueTexture,Vector3(-3, 0, 0),Vector3(2, 2, 0), Vector3(1, 1, 1),true,true);
    //playerLeft.Draw(program);
    
    Entity ballL1=Entity("ball", airBallTexture,Vector3(-2, 0.6, 0),Vector3(0, 0, 0), Vector3(1, 1, 1),true,true);

    Entity ballL2=Entity("ball", airBallTexture,Vector3(-2, -0.6, 0),Vector3(0, 0, 0), Vector3(1, 1, 1),true,true);
    
    Entity ballR1=Entity("ball", iceBallTexture,Vector3(2, 0.6, 0),Vector3(0, 0, 0), Vector3(1, 1, 1),false,true);

    Entity ballR2=Entity("ball", iceBallTexture,Vector3(2, -0.6, 0),Vector3(0, 0, 0), Vector3(1, 1, 1),false,true);

    Entity bar=Entity("bar", barTexture,Vector3(0, 0, 0),Vector3(0, 0, 0), Vector3(1, 1, 1),false,true);
    
    leftScoreVec.push_back(l0);
    leftScoreVec.push_back(l1);
    leftScoreVec.push_back(l2);
    leftScoreVec.push_back(l3);
    
    rightScoreVec.push_back(r0);
    rightScoreVec.push_back(r1);
    rightScoreVec.push_back(r2);
    rightScoreVec.push_back(r3);
    
    //vector<Entity> ballVecLeft;
   // vector<Entity> ballVecRight;
    vector<Entity>ballVec;
    /*vector<Entity>flameVec;
    for(int i=0;i<3;i++)
    {
        flameVec.push_back(Entity("text", flameTexture,Vector3(0, 0, 0),Vector3(0, 0, 0), Vector3(1, 1, 1),true,false));
    }
    int currentFlameIndex=0;
    //vector<Entity>blockVec;*/
    
    bool pLDead=false;
    bool pRDead=false;
    int pLCounter=0;
    int pRCounter=0;
    bool onePlayerMode=false;
    int colorLeft=0;
    int colorRight=1;
    
    ballVec.push_back(ballL1);
    ballVec.push_back(ballL2);
    ballVec.push_back(ballR1);
    ballVec.push_back(ballR2);
    
    int leftScore=0;
    int rightScore=0;
    /*
    double x=-3.5;
    double y=-2;
    for(int i=0;i<24;i++)
    {
        Entity block=Entity("block", blockTexture,Vector3(x, -1.85, 0),Vector3(0, 0, 0), Vector3(1, 1, 1),true,true);
        blockVec.push_back(block);
        Entity block2=Entity("block", blockTexture,Vector3(x, 1.85, 0),Vector3(0, 0, 0), Vector3(1, 1, 1),true,true);
        blockVec.push_back(block2);
        x+=0.3;
        
    }
    for(int i=0;i<15;i++)
    {
        Entity block=Entity("block", blockTexture,Vector3(3.4, y, 0),Vector3(0, 0, 0), Vector3(1, 1, 1),true,true);
        blockVec.push_back(block);
        y+=0.3;
        
    }
    y=-2;
    for(int i=0;i<15;i++)
    {
        Entity block2=Entity("block", blockTexture,Vector3(-3.4,y, 0),Vector3(0, 0, 0), Vector3(1, 1, 1),true,true);
        blockVec.push_back(block2);
        y+=0.3;
    }*/
    
    GameState state=MENU;
    Matrix projectionMatrix;
    Matrix modelMatrix;
    Matrix viewMatrix;
    
    
    projectionMatrix.setOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);
    
    glUseProgram(program.programID);
    SDL_Event event;
    
    float lastFrameTicks = 0.0f;


    bool start=false;
    bool done = false;
    bool gameOver=false;
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 4096 );
    Mix_Chunk *shootSound;
    shootSound = Mix_LoadWAV("shoot.wav");
    
    Mix_Chunk *bounceSound;
    bounceSound = Mix_LoadWAV("cue.wav");
    
    Mix_Chunk *deadSound;
    deadSound = Mix_LoadWAV("fireCombo.wav");
    
    Mix_Music *music;
    music = Mix_LoadMUS( "elemental.mp3" );
    
    Mix_PlayMusic(music, -1);
    
    while (!done) {
        float ticks = (float)SDL_GetTicks()/1000.0f;
        float elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        
        
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
            else if(state==PLAY&&event.type == SDL_KEYDOWN) {
                if(event.key.keysym.scancode==SDL_SCANCODE_SPACE)
                {
                    Mix_PlayChannel( -1, shootSound, 0);
                    
                }
            }
        
        }
        
        
        
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        program.setModelMatrix(modelMatrix);
        program.setProjectionMatrix(projectionMatrix);
        program.setViewMatrix(viewMatrix);
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        if(state==MENU)
        {
            
            DrawText(&program,font
                     , "1 or 2", 0.25, 0);
            if (keys[SDL_SCANCODE_2]) {
                onePlayerMode=false;
                state=PLAY;            }
            else if(keys[SDL_SCANCODE_1]) {
                onePlayerMode=true;
                state=PLAY;            }
            else if(keys[SDL_SCANCODE_LEFT]) {
                colorLeft++;
                if(colorLeft>3)
                {
                    colorLeft=0;
                }
                if(colorLeft==colorRight)
                {
                    colorLeft++;
                    if(colorLeft>3)
                    {
                        colorLeft=0;
                    }
                }
                playerLeft.textureID=cueTexture[colorLeft];
                ballVec[0].textureID=ballTexture[colorLeft];
                ballVec[1].textureID=ballTexture[colorLeft];
                
                
            }
            else if(keys[SDL_SCANCODE_RIGHT]) {
                colorRight++;
            if(colorRight>3)
            {
                colorRight=0;
            }
            if(colorRight==colorLeft)
            {
                colorRight++;
                if(colorRight>3)
                {
                    colorRight=0;
                }
            }
            playerRight.textureID=cueTexture[colorRight];
                ballVec[2].textureID=ballTexture[colorRight];
                ballVec[3].textureID=ballTexture[colorRight];
            /*
                if(colorRight==0)
                {
                    playerRight.textureID=airCueTexture;
                }
                else if(colorRight==1)
                {
                    playerRight.textureID=iceCueTexture;
                }
                else if(colorRight==2)
                {
                    playerRight.textureID=fireCueTexture;
                    //playerRight=Entity("player", fireCueTexture,Vector3(-3, 0, 0),Vector3(2, 2, 0), Vector3(1, 1, 1),true,true);
                }
                else if(colorRight==3)
                {
                    playerRight.textureID=lightningCueTexture;
                }
                */
            }
             
            update(playerLeft, playerRight);
            playerLeft.Draw(program);
            playerRight.Draw(program);
            
        }
        
        else if(state==PLAY)
        {
            
            if(gameOver)
            {
                if(keys[SDL_SCANCODE_SPACE])
                {
                    leftScore=0;
                    rightScore=0;
                    
                    
                    pLCounter=0;
                    pRCounter=0;
                    playerRight.position=Vector3(3, 0, 0);
                    playerLeft.position=Vector3(-3,0,0);
                    
                    ballVec[0].position=Vector3(-2, 0.6, 0);
                    ballVec[1].position=Vector3(-2, -0.6, 0);
                    ballVec[2].position=Vector3(2, 0.6, 0);
                    ballVec[3].position=Vector3(2, -0.6, 0);
                    state=MENU;
                    pLDead=false;
                    pRDead=false;
                    gameOver=false;
                    
                }
            }
            bar.rotation=M_PI;
        bar.Draw(program);
            
        
            
            leftScoreVec[leftScore].Draw(program);
            rightScoreVec[rightScore].Draw(program);
        if(!pRDead)
        {
            if (keys[SDL_SCANCODE_LEFT]) {
                if (playerRight.position.x - playerRight.size.x / 2 > -3.55f)
                    playerRight.position.x-= playerRight.velocity.x * elapsed;
            }
            else if (keys[SDL_SCANCODE_RIGHT]) {
                if (playerRight.position.x + playerRight.size.x / 2< 3.55f)
                    playerRight.position.x+= playerRight.velocity.x * elapsed;
            }
            if (keys[SDL_SCANCODE_UP]) {
                if (playerRight.position.y + playerRight.size.x / 2< 3.55f)
                    playerRight.position.y+= playerRight.velocity.x * elapsed;
            }
            else if (keys[SDL_SCANCODE_DOWN]) {
                if (playerRight.position.y - playerRight.size.x / 2< 3.55f)
                    playerRight.position.y-= playerRight.velocity.x * elapsed;
            }
        }
        else{
            
            pRCounter++;
            Entity("player", barTexture,playerRight.position,Vector3(2, 2, 0), Vector3(1, 1, 1),false,true).Draw(program);
            if(leftScore==3)
            {
                Entity("text", winTexture,playerLeft.position,Vector3(0, 0, 0), Vector3(1, 1, 1),true,true).Draw(program);
                gameOver=true;
            }
            else if(pRCounter==100)
            {
                pRCounter=0;
                pRDead=false;
            }
        }
        
        if(!pLDead&&!onePlayerMode)
        {
            if (keys[SDL_SCANCODE_A]) {
                if (playerLeft.position.x - playerLeft.size.x / 2 > -3.55f)
                    playerLeft.position.x-= playerLeft.velocity.x * elapsed;
            }
            else if (keys[SDL_SCANCODE_D]) {
                if (playerLeft.position.x + playerLeft.size.x / 2< 3.55f)
                    playerLeft.position.x+= playerLeft.velocity.x * elapsed;
            }
            if (keys[SDL_SCANCODE_W]) {
                if (playerLeft.position.y + playerLeft.size.x / 2< 3.55f)
                    playerLeft.position.y+= playerLeft.velocity.x * elapsed;
            }
            else if (keys[SDL_SCANCODE_S]) {
                if (playerLeft.position.y - playerLeft.size.x / 2< 3.55f)
                    playerLeft.position.y-= playerLeft.velocity.x * elapsed;
            }
        }
        else if(onePlayerMode&&!pLDead)
        {
            Entity closeBallFriend=ballVec[0];
            Entity closeBallEnemy=ballVec[2];
            
            if(sqrt(powf((ballVec[2].position.x-playerLeft.position.x),2)+powf((ballVec[2].position.y-playerLeft.position.y),2))>sqrt(powf((ballVec[3].position.x-playerLeft.position.x),2)+powf((ballVec[3].position.y-playerLeft.position.y),2)))
            {
                
                closeBallEnemy=ballVec[3];
            //    closeBallEnemy.position=ballVec[3].position;
            }
            if(sqrt(powf((ballVec[0].position.x-playerLeft.position.x),2)+powf((ballVec[0].position.y-playerLeft.position.y),2))>sqrt(powf((ballVec[1].position.x-playerLeft.position.x),2)+powf((ballVec[1].position.y-playerLeft.position.y),2)))
            {
                closeBallFriend=ballVec[1];
           //     closeBallFriend.position=ballVec[1].position;
            }
            if(ballVec[0].position.x>0&&ballVec[1].position.x<0)
            {
                closeBallFriend=ballVec[1];
            //    closeBallFriend.position=ballVec[1].position;
            }
            
            else if(ballVec[0].position.x<0&&ballVec[1].position.x>0)
            {
                closeBallFriend=ballVec[0];
           //     closeBallFriend.position=ballVec[0].position;
                
            }
            
            
            Entity closestBall=closeBallEnemy;
          //  closestBall.position=closeBallEnemy.position;
            bool closestFriend=false;
            
            if(sqrt(powf((closeBallEnemy.position.x-playerLeft.position.x),2)+powf((closeBallEnemy.position.y-playerLeft.position.y),2))>sqrt(powf((closeBallFriend.position.x-playerLeft.position.x),2)+powf((closeBallFriend.position.y-playerLeft.position.y),2)))
            {
                closestFriend=true;
            }
            
            float filter = 0.05;
            float inverseFilter = 1.0 - filter;
            if(closestFriend&&closeBallFriend.position.y>-10)
            {
                
                
                playerLeft.position = Vector3(
                    closeBallFriend.position.x * filter + playerLeft.position.x * inverseFilter,
                                              closeBallFriend.position.y * filter + playerLeft.position.y * inverseFilter,0);
                //   [self addTargetPosition:CGPointMake(closeBallFriend.position.x,closeBallFriend.position.y)];
            }
            else
            {
                Vector3 pos;
                if(closeBallEnemy.position.x>0)
                {
                    pos=Vector3(
                        -2 * filter + playerLeft.position.x * inverseFilter,
                                0 * filter + playerLeft.position.y * inverseFilter,0);
                    
                }
                else
                {
                    pos = Vector3(
                        (playerLeft.position.x+absolute( closeBallEnemy.position.y-playerLeft.position.y)) * filter + playerLeft.position.x * inverseFilter,
                                  (playerLeft.position.y-absolute(closeBallEnemy.position.x-playerLeft.position.x)) * filter + playerLeft.position.y * inverseFilter,0);
                }
                //Console.log("hi");
                
                 playerLeft.position=pos;
            }
        }
        else{
            pLCounter++;
            Entity("player", barTexture,playerLeft.position,Vector3(2, 2, 0), Vector3(1, 1, 1),false,true).Draw(program);
            if(rightScore>=3)
            {
            Entity("text", winTexture,playerRight.position,Vector3(0, 0, 0), Vector3(1, 1, 1),true,true).Draw(program);
                
                gameOver=true;
            }
            if(pLCounter==100)
            {
                pLCounter=0;
                pLDead=false;
            }
        }
            
            
            update(playerLeft, playerRight);
            if(!gameOver)
            {
            for(int i=0;i<ballVec.size();i++)
            {
                if(collide(playerLeft,ballVec[i],colorLeft,bounceSound)==1)
                {
                    if(!pLDead)
                    {
                        Mix_PlayChannel(-1, deadSound, 0);
                        rightScore++;
                        pLDead=true;
                    }
                }
                
                if(collide(playerRight,ballVec[i],colorRight,bounceSound)==2)
                {
                    if(!pRDead)
                    {
                        Mix_PlayChannel(-1, deadSound, 0);
                        leftScore++;
                        pRDead=true;
                    }
                }
                 
            }
            updateBalls(ballVec, elapsed);
            for(int i=0;i<ballVec.size();i++)
            {
                for(int j=0;j<ballVec.size();j++)
                {
                    if(i!=j)
                    {
                        if(collideBalls(ballVec[i], ballVec[j],lightningBallTexture))
                        {
                            /*
                            if(colorLeft==2)
                            {
                                if(i==0&&j==1)
                                {
                                    flameVec[currentFlameIndex].position=ballVec[i].position;
                                    flameVec[currentFlameIndex].isVisible=true;
                                    currentFlameIndex++;
                                    if(currentFlameIndex>2)
                                    {
                                        currentFlameIndex=0;
                                    }
                                }
                                
                            }
                            else if(colorRight==2)
                            {
                                if(i==2&&j==2)
                                {
                                    flameVec[currentFlameIndex].position=ballVec[i].position;
                                    flameVec[currentFlameIndex].isVisible=true;
                                    currentFlameIndex++;
                                    if(currentFlameIndex>2)
                                    {
                                        currentFlameIndex=0;
                                    }
                                    
                                }
                            }*/
                            if(colorLeft==1)
                            {
                                if((i==0||i==1)&&(j==2||j==3))
                                {
                                    ballVec[j].velocity.x=ballVec[j].velocity.x*0.3;
                                    ballVec[j].velocity.y=ballVec[j].velocity.y*0.3;
                                }
                            }
                            else if(colorRight==1)
                            {
                                if((i==0||i==1)&&(j==2||j==3))
                                {
                                    ballVec[i].velocity.x=ballVec[i].velocity.x*0.3;
                                    ballVec[i].velocity.y=ballVec[i].velocity.y*0.3;
                                }
                            }
                        }
                        
                    }
                }
            }
            }
            
            
            /*
            
            for(int i=0;i<flameVec.size();i++)
            {
                //if(flameVec[i].isVisible)
                {
                    flameVec[i].rotation+=0.1;
                    if(flameVec[i].rotation>1000)
                    {
                        flameVec[i].isVisible=false;
                    }
                    flameVec[i].Draw(program);
                }
            }*/
        playerLeft.Draw(program);
        playerRight.Draw(program);
            
            for(int i=0;i<ballVec.size();i++)
            {
                ballVec[i].Draw(program);
            }
            
            if(gameOver)
            {
                for(int i=0;i<ballVec.size();i++)
                {
                    ballVec[i].velocity=Vector3(0, 0, 0);
                }
            }
           
        
        //enemy.Draw(program);
        //glDisableVertexAttribArray(program.positionAttribute);
        // glDisableVertexAttribArray(program.texCoordAttribute);
        }
        SDL_GL_SwapWindow(displayWindow);
    }
    SDL_Quit();
    return 0;
}
