#include <GL/glut.h>
#include <cmath>

// Variaveis de controle para a animacao
float rotacaoY = 0.0;  // angulo de rotacao no eixo Y
int animacaoAtiva = 1; // Controle da animacao: 1 = ativa, 0 = pausada

// Variaveis para controle da camera
float cameraAngleX = 15.0;  // angulo da camera no eixo X
float cameraAngleY = 30.0;  // angulo da camera no eixo Y
float cameraDist = 30.0;    // Distancia da camera ao centro
int mouseXAnterior = 0;     // ultima posicao X do mouse
int mouseYAnterior = 0;     // ultima posicao Y do mouse
int mouseEsquerdoAtivo = 0; // Indica se o botao esquerdo esta pressionado

float trilhoInferiorZ = 0.0;    // Coordenada Z do centro do trilho inferior
float tamanhoBraco = 2.0;       // Comprimento do braco da garra
float bracoX = 0.0;             // Coordenada X do braco da garra
float anguloGarra = -30.0;      // Angulo de abertura da garra

void inicializa()
{
    glClearColor(0.1, 0.1, 0.1, 1.0);
    glEnable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 1.0, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void desenhaMaquina()
{
    // Base da maquina (cinza) - divide em dois pra fazer o buraco de onde sai o bicho
    glPushMatrix();
    glTranslatef(2.0, -4.0, 0.0);
    glScalef(4.0, 6.0, 8.0);
    glColor3f(0.3, 0.3, 0.3);
    glutSolidCube(1.0);
    glPopMatrix();

    // segundo cubo cinza fundo
    glPushMatrix();
    glTranslatef(-2.0, -4.0, -1.5);
    glScalef(4.0, 6.0, 5.0);
    glColor3f(0.3, 0.8, 0.3);
    glutSolidCube(1.0);
    glPopMatrix();

    // terceito cubo cinza frente
    glPushMatrix();
    glTranslatef(-2.0, -5.5, 2.5);
    glScalef(4.0, 3.0, 3.0);
    glColor3f(0.6, 0.3, 0.3);
    glutSolidCube(1.0);
    glPopMatrix();

    // moldura da parte onde o bicho cai - direita
    glPushMatrix();
    glTranslatef(-3.92, -1.75, 2.5);
    glScalef(0.2, 4.5, 3.0);
    glColor3f(0.6, 0.6, 0.3);
    glutSolidCube(1.0);
    glPopMatrix();

    // moldura da parte onde o bicho cai - esquerda
    glPushMatrix();
    glTranslatef(-0.1, -1.75, 2.5);
    glScalef(0.2, 4.5, 3.0);
    glColor3f(0.6, 0.6, 0.3);
    glutSolidCube(1.0);
    glPopMatrix();

    // moldura da parte onde o bicho cai - tras
    glPushMatrix();
    glTranslatef(-2.0, -1.75, 1.1);
    glScalef(3.6, 4.5, 0.2);
    glColor3f(0.89, 0.5, 0.3);
    glutSolidCube(1.0);
    glPopMatrix();

    // moldura da parte onde o bicho cai - em cima
    glPushMatrix();
    glTranslatef(-2.0, -0.37, 3.9);
    glScalef(3.6, 1.75, 0.2);
    glColor3f(0.8, 0.6, 0.6);
    glutSolidCube(1.0);
    glPopMatrix();

    // moldura da parte onde o bicho cai - em baixo
    glPushMatrix();
    glTranslatef(-2.0, -3.9, 3.9);
    glScalef(3.6, 0.2, 0.2);
    glColor3f(0.8, 0.6, 0.6);
    glutSolidCube(1.0);
    glPopMatrix();

    // lugar onde fica os botoes
    glPushMatrix();
    glTranslatef(2.0, -4.50, 4.75); // Posiciona o bloco na frente da base, alinhado com o chao
    glScalef(4.0, 5.0, 1.5);        // Bloco mais baixo que a base, mas com mesma largura
    glColor3f(1.0, 0.0, 0.0);       // Cor vermelha so pra destacar a gente ver onde ta mexendo
    glutSolidCube(1.0);
    glPopMatrix();

    // botoes da maquina:
    // alavanca (retangulo)

    glPushMatrix();
    glTranslatef(1.0, -1.5, 4.75);
    glScalef(0.2, 1.0, 0.2);
    glColor3f(0.5, 0.0, 0.0); // Cor vermelha
    glutSolidCube(1.0);
    glPopMatrix();

    // Bola da alavanca
    glPushMatrix();
    glTranslatef(1.0, -1.25, 4.75);
    glColor3f(0.9, 0.0, 0.0); // Cor vermelha para a bola
    glutSolidSphere(0.30, 20, 20);
    glPopMatrix();

    // Botao
    glPushMatrix();
    glTranslatef(3.0, -2.0, 4.75);
    glColor3f(0.0, 0.0, 1.0); // Cor azul
    glutSolidSphere(0.4, 20, 20);
    glPopMatrix();

    // Parte superior (caixa transparente)
    glPushMatrix();
    glTranslatef(0.0, 4.0, 0.0);   // Subimos a caixa para 4.0
    glScalef(8.0, 10.0, 8.0);      // Altura aumentada para 10.0
    glColor4f(0.5, 0.5, 0.5, 0.3); // Semitransparente
    glutWireCube(1.0);             // Use SolidCube para a base solida
    glPopMatrix();

    // telhado (cubo vermelho)
    glPushMatrix();
    glTranslatef(0.0, 8.5, 0.0);
    glScalef(8.0, 1.0, 8.0);
    glColor3f(1.0, 0.0, 0.0);
    glutSolidCube(1.0);
    glPopMatrix();

    float rodinhaRaio = 0.7;     // Raio das rodinhas
    float alturaRodinha = -7.75; // Ajustamos para que as rodinhas fiquem mais visiveis

    // Posicoes das rodinhas nas 4 extremidades da base
    glColor3f(0.8, 0.8, 0.8); // Cor das rodinhas (cinza claro)

    // Rodinha 1 - Frente esquerda
    glPushMatrix();
    glTranslatef(-3.0, alturaRodinha, 3.0);
    glutSolidSphere(rodinhaRaio, 20, 20);
    glPopMatrix();

    // Rodinha 2 - Frente direita
    glPushMatrix();
    glTranslatef(3.0, alturaRodinha, 3.0);
    glutSolidSphere(rodinhaRaio, 20, 20);
    glPopMatrix();

    // Rodinha 3 - Tras esquerda
    glPushMatrix();
    glTranslatef(-3.0, alturaRodinha, -3.0);
    glutSolidSphere(rodinhaRaio, 20, 20);
    glPopMatrix();

    // Rodinha 4 - Tras direita
    glPushMatrix();
    glTranslatef(3.0, alturaRodinha, -3.0);
    glutSolidSphere(rodinhaRaio, 20, 20);
    glPopMatrix();
}

void desenhaBrinquedos()
{
    // Brinquedos na base
    for (int i = -2; i <= 2; i++)
    {
        for (int j = -2; j <= 2; j++)
        {
            // Condicao para excluir as bolinhas da frente, do lado esquerdo
            if (i < 1 && j > 0)
            {
                continue; // Pula as bolinhas na frente, do lado esquerdo
            }

            glPushMatrix();
            glTranslatef(i * 1.5, -0.45, j * 1.5);
            glColor3f((i + 3) / 5.0, (j + 3) / 5.0, 0.5); // Cores variadas
            glutSolidSphere(0.6, 20, 20);
            glPopMatrix();
        }
    }
}

void desenhaGarra()
{
    glPushMatrix();
        glRotatef(anguloGarra, 1.0, 0.0, 0.0);
        glPushMatrix();
            glTranslatef(0.0, -0.6, 0.0);
            glScalef(0.2, 1.2, 0.2);
            glutSolidCube(1.0);
        glPopMatrix();
        glPushMatrix();
            glTranslatef(0.0, -1.4, -0.2);
            glRotatef(45.0, 1.0, 0.0, 0.0);
            glScalef(0.2, 0.8, 0.2);
            glutSolidCube(1.0);
        glPopMatrix();
    glPopMatrix();
}

void desenhaBraco()
{
    // trilho superior do braco (fixo)
    glPushMatrix();
        glTranslatef(0.0, 7.8, 0.0);
        glColor3f(0.6, 0.2, 0.1);
        glPushMatrix();
            glScalef(1.0, 1.0, 16.0);
            glutSolidCube(0.5);
        glPopMatrix();

        // trilho inferior (movel)
        glPushMatrix();
            glTranslatef(0.0, -0.5, trilhoInferiorZ);
            glPushMatrix();
                glScalef(16.0, 1.0, 1.0);
                glutSolidCube(0.5);
            glPopMatrix();

            // braco
            glPushMatrix();
                glTranslatef(bracoX, -(tamanhoBraco/2)-0.25, 0.0);
                glColor3f(0.4, 0.4, 0.4);
                glPushMatrix();
                    glScalef(0.5, tamanhoBraco, 0.5);
                    glutSolidCube(1.0);
                glPopMatrix();

                // esfera da garra
                glPushMatrix();
                    glTranslatef(0.0, -(tamanhoBraco/2), 0.0);
                    glColor3f(0.5, 0.5, 0.5);
                    glutSolidSphere(0.5, 20, 20);

                    // hastes da garra
                    glColor3f(0.2, 0.2, 0.8);
                    glPushMatrix();
                        for (int i = 0; i < 4; i++)
                        {
                            glRotatef(90.0*i, 0.0, 1.0, 0.0); // rotacao para formar as 4 garras
                            desenhaGarra();
                        }
                    glPopMatrix();
                glPopMatrix();
            glPopMatrix();
        glPopMatrix();
    glPopMatrix();
}

void desenhaMaquinaBichinhos()
{
    desenhaMaquina();
    desenhaBrinquedos();
    desenhaBraco();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Calcular a posicao da camera com base nos angulos e distancia
    float eyeX = cameraDist * sin(cameraAngleY * M_PI / 180.0) * cos(cameraAngleX * M_PI / 180.0);
    float eyeY = cameraDist * sin(cameraAngleX * M_PI / 180.0);
    float eyeZ = cameraDist * cos(cameraAngleY * M_PI / 180.0) * cos(cameraAngleX * M_PI / 180.0);

    gluLookAt(eyeX, eyeY, eyeZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    // Aplicar rotacao na maquina
    glRotatef(rotacaoY, 0.0, 1.0, 0.0);

    desenhaMaquinaBichinhos();

    glutSwapBuffers();
}

void anima(int valor)
{
    if (animacaoAtiva)
    {
        rotacaoY += 1.0; // Incrementa a rotacao
        if (rotacaoY >= 360.0)
        {
            rotacaoY -= 360.0; // Reinicia o angulo para evitar valores grandes
        }
    }
    glutPostRedisplay();         // Atualiza a tela
    glutTimerFunc(16, anima, 0); // Chama novamente a funcao apos 16ms (~60 FPS)
}

void teclado(unsigned char tecla, int x, int y)
{
    switch (tecla)
    {
    case 'p':
        animacaoAtiva = !animacaoAtiva; // Alterna entre pausar e retomar
        break;
    case 'w':
        if (trilhoInferiorZ > -3.2)
            trilhoInferiorZ -= 0.1;  // move garra para o fundo
        break;
    case 's':
        if (trilhoInferiorZ < 3.2)
            trilhoInferiorZ += 0.1;  // move garra para a frente
        break;
    case 'a':
        if (bracoX > -3.2)
            bracoX -= 0.1;  // move o braco para a esquerda
        break;
    case 'd':
        if (bracoX < 3.2)
            bracoX += 0.1;  // move o braco para a direita
        break;
    case 'r':
        if (tamanhoBraco > 1.0)
            tamanhoBraco -= 0.1;  // retrai o braco, sobe a garra
        break;
    case 'f':
        if (tamanhoBraco < 5.5)
            tamanhoBraco += 0.1;  // extende o braco, desce a garra
        break;
    case 'q':
        if (anguloGarra > -75.0)
            anguloGarra -= 1.0;  // abre a garra
        break;
    case 'e':
        if (anguloGarra < -25.0)
            anguloGarra += 1.0;  // fecha a garra
        break;
    case 27:
        exit(0);
        break;
    }
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            mouseEsquerdoAtivo = 1; // Botao esquerdo pressionado
            mouseXAnterior = x;
            mouseYAnterior = y;
        }
        else if (state == GLUT_UP)
        {
            mouseEsquerdoAtivo = 0; // Botao esquerdo liberado
        }
    }
}

void movimentoMouse(int x, int y)
{
    if (mouseEsquerdoAtivo)
    {
        // Ajusta os angulos da camera com base no movimento do mouse
        cameraAngleY += (x - mouseXAnterior) * 0.2;
        cameraAngleX += (y - mouseYAnterior) * 0.2;

        // Limita o angulo vertical para evitar que a camera vire de cabeca para baixo
        if (cameraAngleX > 89.0)
            cameraAngleX = 89.0;
        if (cameraAngleX < -89.0)
            cameraAngleX = -89.0;

        mouseXAnterior = x;
        mouseYAnterior = y;

        glutPostRedisplay(); // Atualiza a tela
    }
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 800);
    glutCreateWindow("Maquina de Bichinhos");

    inicializa();

    glutDisplayFunc(display);
    glutKeyboardFunc(teclado);
    glutMouseFunc(mouse);
    glutMotionFunc(movimentoMouse); // Adiciona rastreamento de movimento do mouse
    glutTimerFunc(1000, anima, 0);  // Inicia a animacao apos 1 segundo

    glutMainLoop();

    return 0;
}
