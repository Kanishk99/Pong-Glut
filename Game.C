#include <stdio.h>
#include <time.h>
#include <GL/glu.h>
#include <GL/glut.h>

//Pad size
#define padHeight 80
#define padWidth 20

#define MILLISECONDS_PER_FRAME 20

//Ball size
#define ballWidth 20
#define ballHeight 20

//The game window size
#define FIELD_WIDTH 1280
#define FIELD_HEIGHT 720

#define LEFT_SIDE 0
#define RIGHT_SIDE 1

#define LOSE_SERVE_TIME 120 //Timer after each round in ms



typedef struct {
	float x;
	float y;
	int width;
	int height;
	float motionX;
	float motionY;
} Entity;
void RushiKaEasterEgg();
float red=1.0f,green=1.0f,blue=1.0f;
Entity leftPad;
Entity leftBar;
Entity rightPad;
Entity rightBar;
Entity ball;

int loseTimer;

float rectLim=FIELD_HEIGHT;
int purpleScore=0,cyanScore=0;
char purpleBuf[256],cyanBuf[256]; //Strings to store the integers

void renderPaddle(Entity *paddle)
{
	glRectf(paddle->x, paddle->y, paddle->x + paddle->width, paddle->y + paddle->height);
}

//Glut function to display text on window
void bitmapString(void *font, char *s, float x, float y)
{
    char *c;
	glRasterPos2f(x,y);
	for(c=s;*c!='\0';c++) //Text crashes without this
    {
    glutBitmapCharacter(font, *c);
    }
}

//Render all the shapes that need to be on screen(All of type Entity)
void renderFrame(void)
{


	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	//Middle line start
    glBegin(GL_POLYGON);
    glColor3f(1.0f,1.0f,1.0f);
    glVertex2f((FIELD_WIDTH/2)-2, FIELD_HEIGHT);
    glVertex2f((FIELD_WIDTH/2)-2, 0);
    glVertex2f((FIELD_WIDTH/2)+2, 0);
    glVertex2f((FIELD_WIDTH/2)+2, FIELD_HEIGHT);
    glEnd();
    //Middle line end

    RushiKaEasterEgg(); //Easter egg occurs if one team scores more than 4

	glColor3f(1.0f,1.0f,1.0f);
	renderPaddle(&leftPad);
	glColor3f(0.0f, 1.0f, 1.0f); //Cyan
	renderPaddle(&leftBar);
	glColor3f(1.0f,1.0f,1.0f);
	renderPaddle(&rightPad);
	glColor3f(1.0f, 0.0f, 1.0f); //Purple
	renderPaddle(&rightBar);
	glColor3f(red,green,blue);
	renderPaddle(&ball);


	sprintf(cyanBuf, "%d",cyanScore);
	sprintf(purpleBuf, "%d",purpleScore);

	glColor3f(1.0f,1.0f,1.0f);
    bitmapString(GLUT_BITMAP_HELVETICA_18, cyanBuf, (FIELD_WIDTH/2)-25,(FIELD_HEIGHT)-20); //Scoreboard
	bitmapString(GLUT_BITMAP_HELVETICA_18, purpleBuf, (FIELD_WIDTH/2)+25,(FIELD_HEIGHT)-20);
	glutSwapBuffers();
}



void reshapeHandler(int w, int h) //Reshape function for glut(Default)
{
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble) w, 0.0, (GLdouble) h);
}

void initPaddles()
{
	leftPad.x = 0;
	leftPad.y = (FIELD_HEIGHT / 2) - (padHeight / 2); //Central position
	leftPad.width = padWidth;
	leftPad.height = padHeight;

	//Attached next to Pad
	leftBar.x = 15;
	leftBar.y = (FIELD_HEIGHT / 2) - (padHeight / 2);
	leftBar.width = padWidth-15;
	leftBar.height = padHeight;

	rightPad.x = FIELD_WIDTH - padWidth; //Begins exactly at right side this way
	rightPad.y = (FIELD_HEIGHT / 2) - (padHeight / 2);
	rightPad.width = padWidth;
	rightPad.height = padHeight;

	rightBar.x = FIELD_WIDTH - padWidth;
	rightBar.y = (FIELD_HEIGHT / 2) - (padHeight / 2);
	rightBar.width = padWidth-15;
	rightBar.height = padHeight;

	//Ball start position and start speed
	ball.x = (FIELD_WIDTH / 2) - (ballWidth / 2);
	ball.y = (FIELD_HEIGHT / 2) - (ballHeight / 2);

	ball.motionX = 3;
	ball.motionY = 3;
	ball.width = ballWidth;
	ball.height = ballHeight;


}

void keyboardHandler(unsigned char keyChar, int x, int y)
{
	switch (keyChar)
	{
		case 'w':
			leftPad.motionY = 5;
			leftBar.motionY = 5;
			break;

		case 's':
			leftPad.motionY = -5; //Negative number = the pad goes down, the Y position keeps decreasing
			leftBar.motionY = -5;
			break;

		default:
			break;
	}
}
void keyboardHandlerSP(int key,int x,int y)
{
    switch(key)
    {
        case GLUT_KEY_UP:
        rightPad.motionY = 5;
		rightBar.motionY = 5;
		break;

        case GLUT_KEY_DOWN:
		rightPad.motionY = -5;
		rightBar.motionY = -5;
		break;

		default:
		break;
    }
}
void handleLose(int side) //Where the ball should move at beginning of each round
{
	loseTimer = LOSE_SERVE_TIME; //120ms till ball moves when board resets
	initPaddles(); //Reset the screen
	if (side == RIGHT_SIDE) //if either side wins, motion starts the other way
    {
		ball.motionX *= -1;
	}
}

/* yPaddle = distance from bottom of paddle to ball's centre */

float vSpeed(float yPaddle)
 {
	float paddleHalf = padHeight / 2;
	float paddleQuarter = paddleHalf / 2;
	if (yPaddle < paddleQuarter)
    {
		return -4;
	} else if (yPaddle < paddleHalf - 10)
	{
		return -2;
	} else if (yPaddle < paddleHalf + 10)
    {
		return -2;
	} else if (yPaddle < paddleHalf + paddleQuarter)
	{
		return 4;
	}
	return 4;
}

float hSpeed(float yPaddle) {
	float paddleHalf = padHeight / 2;
	float paddleQuarter = paddleHalf / 2;
	//The ball can hit the follow points of the Pad
	if (yPaddle < paddleQuarter) //Edge area less than quartet
    {
		return 8;
	} else if (yPaddle < paddleHalf - 10) //Slightly below half
	{
		return 5;
	} else if (yPaddle < paddleHalf + 10) //Slightly above half
	{
		return 5;
	} else if (yPaddle < paddleHalf + paddleQuarter)
	{
		return 8;
	}
	return 8;
}


void runPhysics()
{
	if (loseTimer > 0) //Starts the countdown after each round in ms
    {
		loseTimer--;
		return;
	}
	ball.x += ball.motionX; //Ball movement animation
	ball.y += ball.motionY;
	if (ball.x + ball.width >= FIELD_WIDTH || ball.x <= 0) //Ball 'bounces' as long as it doesn't reach wdith max and min
    {
		ball.motionX *= -1; //x moves towards opp direction for 'bounce'
	}
	if (ball.y + ball.height >= FIELD_HEIGHT || ball.y <= 0) //The top and bottom parts always bounce
    {
		ball.motionY *= -1; //y moves towards opp direction
	}
	if (ball.x <= leftBar.x + leftBar.width) //If ball touches left bar
    {
		float ballCentre = ball.y + (ball.width / 2); //Get ball centre pos
		if (ballCentre > leftBar.y && ballCentre <= leftBar.y + leftBar.height) //If the ball touches the bar
        {
			float yOffset = ballCentre - leftBar.y;
			ball.motionX = hSpeed(yOffset);
			ball.motionY = vSpeed(yOffset);
			ball.x = leftBar.x + leftBar.width + 1;
			red=0.0f;
			green=1.0f;
		}
		if (ball.x <= 0) //if it goes beyond left x-axis
        {
            purpleScore++;
			printf("Left loses\n");
			handleLose(LEFT_SIDE);
			red=1.0f; //Change ball color to bar color
			green=0.0f;
		}
	}
	if (ball.x + ball.width >= rightPad.x)
    {
		float ballCentre = ball.y + (ball.width / 2);
		if (ballCentre > rightBar.y && ballCentre <= rightBar.y + rightBar.height)
        {
			float yOffset = ballCentre - rightBar.y;
			ball.motionX = -1 * hSpeed(yOffset);
			ball.motionY = vSpeed(yOffset);
			ball.x = rightBar.x - 1 - ball.width;
			red=1.0f;
			green=0.0f;
		}
		if (ball.x + ball.width >= FIELD_WIDTH) //beyond right x-axis
        {
            cyanScore++;
			printf("Right loses\n");
			handleLose(RIGHT_SIDE);
			red=0.0f; //Change ball color to bar color
			green=1.0f;
		}
	}

	float newPaddleY = leftBar.y + leftBar.motionY; //Initialize the motion for left Pad(Coloured part is Bar)
	float newPaddlePY = leftPad.y+leftPad.motionY;
	if (newPaddleY < 0 || newPaddleY + leftBar.height >= FIELD_HEIGHT) //If it reaches height limit automatically stop the Pad(0)
    {
		leftBar.motionY=0;
		leftPad.motionY=0;
	}
	else//else it moves till the limit reaches
	{
		leftBar.y=newPaddleY;
		leftPad.y=newPaddlePY;
	}

	newPaddleY = rightBar.y + rightBar.motionY; //Initialize the motion for right Pad(Coloured part is Bar)
	newPaddlePY = rightPad.y + rightPad.motionY;

	if (newPaddleY < 0 || newPaddleY + rightBar.height >= FIELD_HEIGHT) //If it reaches height limit automatically stop the Pad(0)
	{
        rightBar.motionY = 0;
		rightPad.motionY = 0;
	}
	else//else it moves till the limit reaches
	{
		rightBar.y = newPaddleY;
		rightPad.y = newPaddlePY;
	}
}

void RushiKaEasterEgg()
{
    if(purpleScore==5)
    {
        glColor3f(1.0,1.0,1.0);
        glBegin(GL_LINE_LOOP);
        glVertex2f((FIELD_WIDTH-100),FIELD_HEIGHT);
        glVertex2f((FIELD_WIDTH-100),FIELD_HEIGHT-50);
        glVertex2f((FIELD_WIDTH-120),FIELD_HEIGHT-50);
        glVertex2f((FIELD_WIDTH-120),FIELD_HEIGHT-70);
        glVertex2f((FIELD_WIDTH-100),FIELD_HEIGHT-70);
        glVertex2f((FIELD_WIDTH-100),FIELD_HEIGHT-50);
        glVertex2f((FIELD_WIDTH-80),FIELD_HEIGHT-50);
        glVertex2f((FIELD_WIDTH-80),FIELD_HEIGHT-70);
        glVertex2f((FIELD_WIDTH-60),FIELD_HEIGHT-70);
        glVertex2f((FIELD_WIDTH-60),FIELD_HEIGHT-50);
        glVertex2f((FIELD_WIDTH-80),FIELD_HEIGHT-50);
        glVertex2f((FIELD_WIDTH-80),FIELD_HEIGHT);
        glVertex2f((FIELD_WIDTH-100),FIELD_HEIGHT);
        glVertex2f((FIELD_WIDTH-100),FIELD_HEIGHT-20);
        glVertex2f((FIELD_WIDTH-100),FIELD_HEIGHT-20);
        glVertex2f((FIELD_WIDTH-80),FIELD_HEIGHT-20);
        glVertex2f((FIELD_WIDTH-80),FIELD_HEIGHT);
        glEnd();
    }
    else if(cyanScore==5)
    {
        glColor3f(1.0,1.0,1.0);
        glBegin(GL_LINE_LOOP);
        glVertex2f((100),FIELD_HEIGHT);
        glVertex2f((100),FIELD_HEIGHT-50);
        glVertex2f((120),FIELD_HEIGHT-50);
        glVertex2f((120),FIELD_HEIGHT-70);
        glVertex2f((100),FIELD_HEIGHT-70);
        glVertex2f((100),FIELD_HEIGHT-50);
        glVertex2f((80),FIELD_HEIGHT-50);
        glVertex2f((80),FIELD_HEIGHT-70);
        glVertex2f((60),FIELD_HEIGHT-70);
        glVertex2f((60),FIELD_HEIGHT-50);
        glVertex2f((80),FIELD_HEIGHT-50);
        glVertex2f((80),FIELD_HEIGHT);
        glVertex2f((100),FIELD_HEIGHT);
        glVertex2f((100),FIELD_HEIGHT-20);
        glVertex2f((100),FIELD_HEIGHT-20);
        glVertex2f((80),FIELD_HEIGHT-20);
        glVertex2f((80),FIELD_HEIGHT);
        glEnd();
    }
}
//Easter egg ends here, rushi ko khelne yaad se bol

//Default timer function,  Physics and other animations are called every 1ms
void frameTimerCallback() //Timer callback function for glut, the animation will work in frames/ms
{
    glRasterPos2i(100, 120);
    glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
	runPhysics();
	glutTimerFunc(MILLISECONDS_PER_FRAME, frameTimerCallback, 0); //Animation frames per second
	glutPostRedisplay();
}


int main(int argc, char** argv)
{
	initPaddles();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(FIELD_WIDTH, FIELD_HEIGHT);
	glutCreateWindow("Pong");

	glutDisplayFunc(renderFrame);
	glutReshapeFunc(reshapeHandler);

	glutKeyboardFunc(keyboardHandler);
	glutSpecialFunc(keyboardHandlerSP);
	glutTimerFunc(MILLISECONDS_PER_FRAME, frameTimerCallback, 0);
	/*If this is showing an error double click the warning below it, freeglut_std.h opens automatically, then change
	FGAPI void    FGAPIENTRY glutTimerFunc( unsigned int time, void (* callback)(int), int value );
	to
	FGAPI void    FGAPIENTRY glutTimerFunc( unsigned int time, void (* callback)( ), int value );
	Then save freeglut_std.h
    */
	glutMainLoop();
}

