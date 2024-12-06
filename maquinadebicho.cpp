#include <GL/glut.h>
#include <cmath>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// Variaveis de controle da janela e dos viewports
float larguraCena = 1000;
float larguraControles = 0;
float larguraJanela = larguraCena + larguraControles;
float alturaJanela = 800;

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

// Variaveis para controle do braco mecanico
float trilhoInferiorZ = 0.0;    // Coordenada Z do centro do trilho inferior
float tamanhoBraco = 2.0;       // Comprimento do braco da garra
float bracoX = 0.0;             // Coordenada X do braco da garra
float anguloGarra = -30.0;      // Angulo de abertura da garra

// Variaveis de controle da iluminacao
float nivelBrilho = 25.0;  // brilho medio
bool lampadaAcesa;

GLuint idsTextura[5];           // ids das texturas, 0 para parede, 1 para chao, etc

void carregarTextura(const char* nomeArquivo, int indice)
{
 int largura, altura, canais;

 unsigned char *dados = stbi_load(nomeArquivo, &largura, &altura, &canais, 0);

 if (!dados)
  exit(1);

 // gerar textura
 glGenTextures(1, &idsTextura[indice]);
 glBindTexture(GL_TEXTURE_2D, idsTextura[indice]);

 // configurar parametros da textura
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

 glTexImage2D(GL_TEXTURE_2D, 0, (canais == 4) ? GL_RGBA : GL_RGB,
               largura, altura, 0, (canais == 4) ? GL_RGBA : GL_RGB,
               GL_UNSIGNED_BYTE, dados);

 // liberar a memoria da imagem
 stbi_image_free(dados);
}

void inicializa()
{
    glClearColor(0.1, 0.1, 0.1, 1.0);

    glEnable(GL_DEPTH_TEST);                            // Habilitar teste de profundidade
    //glEnable(GL_TEXTURE_2D);

    carregarTextura("texturas/parede.png", 0);          // Textura da parede frontal
    carregarTextura("texturas/piso.png", 1);
    carregarTextura("texturas/paredeesquerda.png", 2);
    carregarTextura("texturas/controles1.png", 3);

    // ativar a iluminacao
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    lampadaAcesa = true;
    glEnable(GL_LIGHT1);
    
    // configura o material
    //glEnable(GL_COLOR_MATERIAL); // permite o controle da cor com glColor3f
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    // configura sombreamento
    glShadeModel(GL_SMOOTH);
}

void configuraIluminacao()
{
    // Luz 0 e a lampada que fica dentro da maquina
    GLfloat luzAmbiente1[] = {0.1, 0.1, 0.1, 1.0};   // luz suave
    GLfloat luzDifusa1[] = {1.0, 1.0, 1.0, 1.0};     // luz branca
    glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmbiente1);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa1);

    // Luz 1 fica fora da maquina entao tem posicao estatica
    GLfloat luzAmbiente2[] = {0.1, 0.1, 0.1, 1.0};   // luz suave
    GLfloat luzDifusa2[] = {1.0, 1.0, 1.0, 1.0};     // luz branca
    GLfloat luzPosicao[] = {8.0, 10.0, 8.0, 1.0};    // luz pontual
    glLightfv(GL_LIGHT1, GL_AMBIENT, luzAmbiente2);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, luzDifusa2);
    glLightfv(GL_LIGHT1, GL_POSITION, luzPosicao);

    /*
    glPushMatrix();
        glTranslatef(8.0, 10.0, 8.0);
        glutSolidTeapot(1.0);
    glPopMatrix();
    */
}

void configuraMaterial(float r, float g, float b)  // Red, Green, Blue
{
    GLfloat materialDifuso[] = {r, g, b, 1.0};          // cor
    GLfloat materialAmbiente[] = {r/5, g/5, b/5, 1.0};  // cor escura
    GLfloat materialEspecular[] = {0.5, 0.5, 0.5, 1.0};

    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialDifuso);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, materialAmbiente);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materialEspecular);

    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, nivelBrilho);
}

void configuraMaterialFosco(float r, float g, float b)  // Red, Green, Blue
{
    GLfloat materialDifuso[] = {r, g, b, 1.0};          // cor
    GLfloat materialAmbiente[] = {r/5, g/5, b/5, 1.0};  // cor escura
    GLfloat materialEspecular[] = {0.0, 0.0, 0.0, 1.0};

    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialDifuso);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, materialAmbiente);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materialEspecular);

    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, nivelBrilho);
}

void configuraMaterialLampada() {
    GLfloat materialDifuso[] = {1.0, 1.0, 1.0, 1.0};    // branco
    GLfloat materialAmbiente[] = {1.0, 1.0, 1.0, 1.0};  // branco
    GLfloat materialEspecular[] = {0.0, 0.0, 0.0, 1.0};

    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialDifuso);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, materialAmbiente);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materialEspecular);

    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 5.0);
}

void desenhaParede()
{
    glColor3f(1.0f, 1.0f, 1.0f);                    // Define a cor para branco (nao sei pq mas so funcionou quando colouqei
    glBindTexture(GL_TEXTURE_2D, idsTextura[0]);    // Aplica a textura da parede
    glPushMatrix();

    glTranslatef(0.0, 6.0, -25.0);

    glBegin(GL_QUADS);
    // Vertices da parede traseira
    glTexCoord2f(0.0, 0.0); glVertex3f(-30.0, -15.0, -4.1);          // Inferior esquerdo
    glTexCoord2f(1.0, 0.0); glVertex3f(30.0, -15.0, -4.1);           // Inferior direito
    glTexCoord2f(1.0, 1.0); glVertex3f(30.0, 40.0, -4.1);            // Superior direito
    glTexCoord2f(0.0, 1.0); glVertex3f(-30.0, 40.0, -4.1);           // Superior esquerdo
    glEnd();

    glPopMatrix();
}

void desenhaChao()
{
    glBindTexture(GL_TEXTURE_2D, idsTextura[1]);            // Aplica a textura do chao

    glPushMatrix();

    // Transladar o chao para baixo, se necessario
    glTranslatef(0.0, -8.5, 0.0);
    glBegin(GL_QUADS);
    // Vertices do chao
    glTexCoord2f(0.0, 0.0); glVertex3f(-30.0, 0.0, 30.0);  // Superior esquerdo
    glTexCoord2f(3.0, 0.0); glVertex3f(30.0, 0.0, 30.0);   // Superior direito
    glTexCoord2f(3.0, 3.0); glVertex3f(30.0, 0.0, -30.0);  // Inferior direito
    glTexCoord2f(0.0, 3.0); glVertex3f(-30.0, 0.0, -30.0); // Inferior esquerdo
    glEnd();

    glPopMatrix();
}

void desenhaParedeEsquerda()
{
    glColor3f(1.0f, 1.0f, 1.0f);
    glBindTexture(GL_TEXTURE_2D, idsTextura[2]);
    glPushMatrix();

    glTranslatef(-25.0, 6.0, 1.0);
    glRotatef(90.0, 0.0, 1.0, 0.0);                                  // Rotaciona a parede para ficar perpendicular ao eixo Z

    glBegin(GL_QUADS);
    // Vertices da parede traseira
    glTexCoord2f(0.0, 0.0); glVertex3f(-30.0, -15.0, -4.1);          // Inferior esquerdo
    glTexCoord2f(1.0, 0.0); glVertex3f(30.0, -15.0, -4.1);           // Inferior direito
    glTexCoord2f(1.0, 1.0); glVertex3f(30.0, 40.0, -4.1);            // Superior direito
    glTexCoord2f(0.0, 1.0); glVertex3f(-30.0, 40.0, -4.1);           // Superior esquerdo
    glEnd();

    glPopMatrix();
}

void desenhaParedeDireita()
{
    glColor3f(1.0f, 1.0f, 1.0f);
    glBindTexture(GL_TEXTURE_2D, idsTextura[2]);
    glPushMatrix();

    // Ajuste de posicionamento e rotacao
    glTranslatef(25.0, 6.0, 0.0);  // Move para a posicao correta (lado direito)
    glRotatef(-90.0, 0.0, 1.0, 0.0); // Rotaciona para alinhar com o plano Z

    glBegin(GL_QUADS);
        // Definicao dos vertices e coordenadas de textura
        glTexCoord2f(0.0, 0.0); glVertex3f(-30.0, -15.0, -4.1); // Inferior esquerdo
        glTexCoord2f(1.0, 0.0); glVertex3f(30.0, -15.0, -4.1);  // Inferior direito
        glTexCoord2f(1.0, 1.0); glVertex3f(30.0, 40.0, -4.1);   // Superior direito
        glTexCoord2f(0.0, 1.0); glVertex3f(-30.0, 40.0, -4.1);  // Superior esquerdo
    glEnd();

    glPopMatrix();
}

void desenhaParedeTraseira()
{
    glColor3f(1.0f, 1.0f, 1.0f); // Define a cor para branco
    glBindTexture(GL_TEXTURE_2D, idsTextura[2]); // Aplica a textura da parede traseira
    glPushMatrix();

    // Posiciona no fundo da cena
    glTranslatef(0.0, 6.0, 30.0); // Ajuste da posicao no eixo Z

    glBegin(GL_QUADS);
        // Definicao dos vertices e coordenadas de textura
        glTexCoord2f(0.0, 0.0); glVertex3f(-30.0, -15.0, 0.0); // Inferior esquerdo
        glTexCoord2f(1.0, 0.0); glVertex3f(30.0, -15.0, 0.0);  // Inferior direito
        glTexCoord2f(1.0, 1.0); glVertex3f(30.0, 40.0, 0.0);   // Superior direito
        glTexCoord2f(0.0, 1.0); glVertex3f(-30.0, 40.0, 0.0);  // Superior esquerdo
    glEnd();

    glPopMatrix();
}

void desenhaMaquina()
{
    //glColor3f(1.0, 0.0, 0.0);  // vermelho
    glDisable(GL_COLOR_MATERIAL); // permite o controle da cor com glColor3f

    configuraMaterial(1.0, 0.0, 0.0);  // vermelho forte

    // Base da maquina - divide em dois pra fazer o buraco de onde sai o bicho
    glPushMatrix();
        glTranslatef(2.0, -4.0, 0.0);
        glScalef(4.0, 6.0, 8.0);
        glutSolidCube(1.0);
    glPopMatrix();

    // segundo cubo
    glPushMatrix();
        glTranslatef(-2.0, -4.0, -1.5);
        glScalef(4.0, 6.0, 5.0);
        glutSolidCube(1.0);
    glPopMatrix();

    // terceito cubo
    glPushMatrix();
        glTranslatef(-2.0, -5.5, 2.5);
        glScalef(4.0, 3.0, 3.0);
        glutSolidCube(1.0);
    glPopMatrix();

    configuraMaterialFosco(0.1, 0.0, 0.0);  // vermelho escuro

    // moldura da parte onde o bicho cai - direita
    glPushMatrix();
        glTranslatef(-3.92, -1.75, 2.5);
        glScalef(0.2, 4.5, 3.0);
        glutSolidCube(1.0);
    glPopMatrix();

    // moldura da parte onde o bicho cai - esquerda
    glPushMatrix();
        glTranslatef(-0.1, -1.75, 2.5);
        glScalef(0.2, 4.5, 3.0);
        glutSolidCube(1.0);
    glPopMatrix();

    //glColor3f(0.4, 0.0, 0.0);  // vermelho escuro
    //configuraMaterial(0.4, 0.0, 0.0);  // vermelho escuro
    configuraMaterialFosco(0.05, 0.0, 0.0);  // vermelho escuro

    // moldura da parte onde o bicho cai - tras
    glPushMatrix();
        glTranslatef(-2.0, -1.75, 1.1);
        glScalef(3.6, 4.5, 0.2);
        glutSolidCube(1.0);
    glPopMatrix();

    //glColor3f(0.8, 0.8, 0.0);  // amarelo
    //configuraMaterial(0.8, 0.8, 0.0);  // amarelo
    configuraMaterialFosco(0.4, 0.4, 0.0);  // amarelo
    
    // moldura da parte onde o bicho cai - em cima
    glPushMatrix();
        glTranslatef(-2.0, -0.37, 3.9);
        glScalef(3.6, 1.75, 0.2);
        glutSolidCube(1.0);
    glPopMatrix();

    // moldura da parte onde o bicho cai - em baixo
    glPushMatrix();
        glTranslatef(-2.0, -3.9, 3.9);
        glScalef(3.6, 0.2, 0.2);
        glutSolidCube(1.0);
    glPopMatrix();

    //glColor3f(0.5, 0.0, 0.0);  // vermelho medio
    configuraMaterial(0.5, 0.0, 0.0);  // vermelho medio
    
    // lugar onde fica os botoes
    glPushMatrix();
        glTranslatef(2.0, -4.50, 4.75);
        glScalef(4.0, 5.0, 1.5);
        glutSolidCube(1.0);
    glPopMatrix();

    //glColor3f(0.0, 0.0, 0.3);  // azul escuro
    //configuraMaterial(0.0, 0.0, 0.3);  // azul escuro
    configuraMaterialFosco(0.0, 0.0, 0.1);  // azul escuro
    
    // botoes da maquina:
    // alavanca (retangulo)
    glPushMatrix();
        glTranslatef(1.0, -1.5, 4.75);
        glScalef(0.2, 1.0, 0.2);
        glutSolidCube(1.0);
    glPopMatrix();

    //glColor3f(0.0, 0.0, 0.8);  // azul claro
    configuraMaterial(0.0, 0.0, 0.8);  // azul claro
    
    // Bola da alavanca
    glPushMatrix();
        glTranslatef(1.0, -1.25, 4.75);
        glutSolidSphere(0.30, 20, 20);
    glPopMatrix();

    // Botao
    glPushMatrix();
        glTranslatef(3.0, -2.0, 4.75);
        glutSolidSphere(0.4, 20, 20);
    glPopMatrix();

    // Parte superior (caixa transparente)
    glPushMatrix();
        glTranslatef(0.0, 4.0, 0.0);
        glScalef(8.0, 10.0, 8.0);
        glColor4f(0.5, 0.5, 0.5, 0.3);    //transparente
        glutWireCube(1.0);
    glPopMatrix();

    //glColor3f(1.0, 0.0, 0.0);  // vermelho
    configuraMaterial(1.0, 0.0, 0.0);  // vermelho forte
    
    // telhado (cubo vermelho)
    glPushMatrix();
        glTranslatef(0.0, 8.5, 0.0);
        glScalef(8.0, 1.0, 8.0);
        glutSolidCube(1.0);
    glPopMatrix();

    float rodinhaRaio = 0.7;        // Raio das rodinhas
    float alturaRodinha = -7.75;    // Ajustamos para que as rodinhas fiquem mais visiveis

    // Posicoes das rodinhas nas 4 extremidades da base

    //glColor3f(0.8, 0.8, 0.8); // Cor das rodinhas (cinza claro)
    configuraMaterial(0.8, 0.8, 0.8);  // Cor das rodinhas (cinza claro)

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

void desenhaMaquinadetalhes()
{
    glDisable(GL_COLOR_MATERIAL); // permite o controle da cor com glColor3f

    //glColor3f(1.0, 0.0, 0.0);  // vermelho
    configuraMaterial(1.0, 0.0, 0.0);  // vermelho forte
    
    // Ponta do foguete 1 (frontal direita)
    glPushMatrix();
        glTranslatef(4.0, -7.0, 3.5);
        glScalef(1.5, 1.5, 1.5);
        glRotatef(-90, 1.0, 0.0, 0.0);
        glutSolidCone(1.5, 4.0, 50, 50);
    glPopMatrix();

    // Ponta do foguete 2 (frontal esquerda)
    glPushMatrix();
        glTranslatef(-4.0, -7.0, 3.5);
        glScalef(1.5, 1.5, 1.5);
        glRotatef(-90, 1.0, 0.0, 0.0);
        glutSolidCone(1.5, 4.0, 50, 50);
    glPopMatrix();

    // Ponta do foguete 3 (traseira direita)
    glPushMatrix();
        glTranslatef(4.0, -7.0, -3.5);
        glScalef(1.5, 1.5, 1.5);
        glRotatef(-90, 1.0, 0.0, 0.0);
        glutSolidCone(1.5, 4.0, 50, 50);
    glPopMatrix();

    // Ponta do foguete 4 (traseira esquerda)
    glPushMatrix();
        glTranslatef(-4.0, -7.0, -3.5);
        glScalef(1.5, 1.5, 1.5);
        glRotatef(-90, 1.0, 0.0, 0.0);
        glutSolidCone(1.5, 4.0, 50, 50);
    glPopMatrix();

    //telhado triangular frontal
    glPushMatrix();
        glBegin(GL_TRIANGLES);
        glVertex3f(-4.0, 9.0, 4.0);
        glVertex3f(4.0, 9.0, 4.0);
        glVertex3f(0.0, 13.0, 0.0);
        glEnd();
    glPopMatrix();

    //telhado triangular direito
    glPushMatrix();
        glBegin(GL_TRIANGLES);
        glVertex3f(4.0, 9.0, 4.0);
        glVertex3f(4.0, 9.0, -4.0);
        glVertex3f(0.0, 13.0, 0.0);
        glEnd();
    glPopMatrix();

    //telhado triangular esquerdo
    glPushMatrix();
        glBegin(GL_TRIANGLES);
        glVertex3f(-4.0, 9.0, 4.0);
        glVertex3f(-4.0, 9.0, -4.0);
        glVertex3f(0.0, 13.0, 0.0);
        glEnd();
    glPopMatrix();

    //telhado triangular traseiro
    glPushMatrix();
        glBegin(GL_TRIANGLES);
        glVertex3f(-4.0, 9.0, -4.0);
        glVertex3f(4.0, 9.0, -4.0);
        glVertex3f(0.0, 13.0, 0.0);
        glEnd();
    glPopMatrix();
}

void desenhaBrinquedos()
{
    glEnable(GL_COLOR_MATERIAL); // permite o controle da cor com glColor3f

    // Fileira da base (nivel 1)
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

    // Segunda fileira direita
    glPushMatrix();
    glTranslatef(-0.8, -0.4, -1.5); // Move a segunda fileira 2 unidades para tras
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            glPushMatrix();
            glTranslatef(i * 1.5, 0.75, j * 1.5); // Altura incrementada
            glColor3f((i + 2) / 5.0, (j + 2) / 5.0, 0.8); // Cores levemente diferentes
            glutSolidSphere(0.6, 20, 20);
            glPopMatrix();
        }
    }
    glPopMatrix();

    //Terceira fileira esquerda (1 coluna, 4 bolinhas)

    glPushMatrix();
    glTranslatef(2.3, 0.3, -2.1); // Translada a coluna no eixo Z (opcional, pode remover ou ajustar)

    for (int k = 0; k < 4; k++) // 4 bolinhas na coluna
    {
        glPushMatrix();
        glTranslatef(0.0, 0.0, k * 1.5); // Move as bolinhas ao longo do eixo Z
        glColor3f(0.5, (k + 1) / 4.0, 0.8); // Cores variadas (com base no indice k)
        glutSolidSphere(0.6, 20, 20);
        glPopMatrix();
    }

    glPopMatrix();

    glDisable(GL_COLOR_MATERIAL); // permite o controle da cor com glColor3f
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
    glDisable(GL_COLOR_MATERIAL); // permite o controle da cor com glColor3f

    // trilho superior do braco (fixo)
    glPushMatrix();
        //glColor3f(0.8, 0.8, 0.8);  // cinza claro
        //configuraMaterial(0.8, 0.8, 0.8);  // cinza claro
        configuraMaterialFosco(0.3, 0.3, 0.3);  // cinza claro

        glTranslatef(0.0, 7.7, 0.0);
        glPushMatrix();
            glPushMatrix();
                glScalef(1.0, 1.0, 16.0);
                glutSolidCube(0.5);
            glPopMatrix();

            // lampada branca
            glPushMatrix();
                configuraMaterialLampada();  // branco
                glTranslatef(3.5, 0.3, 3.5);
                glutSolidSphere(0.2, 20, 20);

                // Posiciona a luz pontual aqui
                GLfloat luzPosicao[] = {0.0, -0.4, 0.0, 1.0};    // luz pontual
                glLightfv(GL_LIGHT0, GL_POSITION, luzPosicao);

                /*glPushMatrix();
                    glColor3f(1.0, 1.0, 0.0);
                    glTranslatef(0.0, -0.3, 0.0);
                    glutSolidTeapot(0.4);
                glPopMatrix();*/
            glPopMatrix();
        glPopMatrix();

        configuraMaterialFosco(0.3, 0.3, 0.3);  // cinza claro

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
                glPushMatrix();
                    glScalef(0.5, tamanhoBraco, 0.5);
                    glutSolidCube(1.0);
                glPopMatrix();

                // esfera da garra
                glPushMatrix();
                    //glColor3f(0.4, 0.4, 0.4);  // cinza escuro
                    //configuraMaterial(0.4, 0.4, 0.4);  // cinza escuro
                    configuraMaterialFosco(0.2, 0.2, 0.2);  // cinza escuro

                    glTranslatef(0.0, -(tamanhoBraco/2), 0.0);
                    glutSolidSphere(0.5, 20, 20);

                    // hastes da garra
                    //glColor3f(0.0, 0.0, 1.8);  // azul
                    //configuraMaterial(0.0, 0.0, 1.0);  // azul forte
                    configuraMaterialFosco(0.0, 0.0, 0.5);  // azul forte

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
    desenhaBrinquedos();
    desenhaBraco();
    desenhaMaquina();
    desenhaMaquinadetalhes();
}

void desenhaMaquinas()
{
    glDisable(GL_COLOR_MATERIAL); // permite o controle da cor com glColor3f

    // Desenha a maquina central com rotacao
    glPushMatrix();
        glRotatef(rotacaoY, 0.0, 1.0, 0.0);  // Aplica a rotacao somente a maquina central
        desenhaMaquinaBichinhos();  // Desenha a maquina
    glPopMatrix();

    // Desenha as maquinas do lado direito (sem rotacao)
    glPushMatrix();
        glTranslatef(20.0, 0.0, -15.0);  // Move a maquina para o lado direito
        glRotatef(-90.0, 0.0, 1.0, 0.0);
        desenhaMaquina();
        desenhaMaquinadetalhes();
        desenhaBrinquedos();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(20.0, 0.0, 2.0);  // Move a maquina para o lado direito
        glRotatef(-90.0, 0.0, 1.0, 0.0);
        desenhaMaquina();
        desenhaMaquinadetalhes();
        desenhaBrinquedos();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(20.0, 0.0, 20.0);  // Move a maquina para o outro lado direito
        glRotatef(-90.0, 0.0, 1.0, 0.0);
        desenhaMaquina();
        desenhaMaquinadetalhes();
        desenhaBrinquedos();
    glPopMatrix();

    // Desenha as maquinas do lado esquerdo (sem rotacao)

    glPushMatrix();
        glTranslatef(-20.0, 0.0, -15.0);
        glRotatef(90.0, 0.0, 1.0, 0.0);
        desenhaMaquina();
        desenhaMaquinadetalhes();
        desenhaBrinquedos();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(-20.0, 0.0, 2.0);
        glRotatef(90.0, 0.0, 1.0, 0.0);
        desenhaMaquina();
        desenhaMaquinadetalhes();
        desenhaBrinquedos();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(-20.0, 0.0, 20.0);
        glRotatef(90.0, 0.0, 1.0, 0.0);
        desenhaMaquina();
        desenhaMaquinadetalhes();
        desenhaBrinquedos();
    glPopMatrix();
}

void desenhaCena()
{
    glViewport(0, 0, larguraCena, alturaJanela);  // Define a nova area de visualizacao
    glMatrixMode(GL_PROJECTION);        // Definir a matriz de projecao
    glLoadIdentity();                   // Limpar a matriz de projecao
    gluPerspective(45.0, (GLfloat)larguraCena / (GLfloat)alturaJanela, 1.0, 100.0); // Ajusta a perspectiva
    glMatrixMode(GL_MODELVIEW);         // Volta para a matriz de modelagem

    glLoadIdentity();

    // Calcular a posicao da camera com base nos angulos e distancia
    float eyeX = cameraDist * sin(cameraAngleY * M_PI / 180.0) * cos(cameraAngleX * M_PI / 180.0);
    float eyeY = cameraDist * sin(cameraAngleX * M_PI / 180.0);
    float eyeZ = cameraDist * cos(cameraAngleY * M_PI / 180.0) * cos(cameraAngleX * M_PI / 180.0);

    if (eyeY < -2.0) eyeY = -2.0;   // LIMITAR A ALTURA DA CAMERA P/ NAO PASSAR DO CHAO

    gluLookAt(eyeX, eyeY, eyeZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    configuraIluminacao();

    // Aplicar rotacao na maquina
    //glRotatef(rotacaoY, 0.0, 1.0, 0.0);

    //desenhaMaquinaBichinhos();
    desenhaMaquinas();

    glEnable(GL_COLOR_MATERIAL); // permite o controle da cor com glColor3f
    
    // Habilitar o mapeamento de textura
    glEnable(GL_TEXTURE_2D);

    desenhaParede();
    desenhaChao();
    desenhaParedeEsquerda();
    desenhaParedeDireita();
    desenhaParedeTraseira();

    // Desabilitar o mapeamento de textura
    glDisable(GL_TEXTURE_2D);

    glDisable(GL_COLOR_MATERIAL); // permite o controle da cor com glColor3f
}

void desenhaControles()
{
    glViewport(larguraCena, 0, larguraControles, alturaJanela);  // Define a nova area de visualizacao
    glMatrixMode(GL_PROJECTION);        // Definir a matriz de projecao
    glLoadIdentity();                   // Limpar a matriz de projecao
    gluPerspective(45.0, (GLfloat)larguraControles / (GLfloat)alturaJanela, 1.0, 100.0); // Ajusta a perspectiva
    glMatrixMode(GL_MODELVIEW);         // Volta para a matriz de modelagem

    glLoadIdentity();

    //glTranslatef(110.0, 110.0, 110.0);

    gluLookAt(110.0, 110.0, 110.0, 110.0, 110.0, 130.0, 0.0, 1.0, 0.0);

    glTranslatef(110.0, 110.0, 130.0);

    // Habilitar o mapeamento de textura
    glEnable(GL_TEXTURE_2D);

    glColor3f(1.0f, 1.0f, 1.0f);                    // Define a cor para branco (nao sei pq mas so funcionou quando colouqei
    glBindTexture(GL_TEXTURE_2D, idsTextura[3]);    // Aplica a textura da parede
    
    glPushMatrix();

    //glTranslatef(-5.0, -10.0, 0.0);

    glBegin(GL_QUADS);
    // Vertices da parede traseira
    glTexCoord2f(1.0, 1.0); glVertex3f(-3.1, -7.5, 0.0);          // Inferior esquerdo vira superior direito
    glTexCoord2f(0.0, 1.0); glVertex3f(3.2, -7.5, 0.0);           // Inferior direito vira superior esquerdo
    glTexCoord2f(0.0, 0.0); glVertex3f(3.2, 8.5, 0.0);            // Superior direito vira inferior esquerdo
    glTexCoord2f(1.0, 0.0); glVertex3f(-3.1, 8.5, 0.0);           // Superior esquerdo vira inferior direito
    glEnd();

    glPopMatrix();

    // Desabilitar o mapeamento de textura
    glDisable(GL_TEXTURE_2D);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    desenhaCena();
    desenhaControles();
    
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
        if (tamanhoBraco < 4.2)
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
        case 't':
        // reseta a camera
        rotacaoY = 0.0;
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
            mouseEsquerdoAtivo = 1;                                     // Botao esquerdo pressionado
            mouseXAnterior = x;
            mouseYAnterior = y;
        }
        else if (state == GLUT_UP)
        {
            mouseEsquerdoAtivo = 0;                                     // Botao esquerdo liberado
        }
    }
    else if (button == 3)                                           // Scroll para cima
    {
        cameraDist -= 1.0f;
        if (cameraDist < 5.0f) cameraDist = 5.0f;
        glutPostRedisplay();
    }
    else if (button == 4)                                         // Scroll para baixo
    {
        cameraDist += 1.0f;
        if (cameraDist > 30.0f) cameraDist = 30.0f;
        glutPostRedisplay();
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
        if (cameraAngleX < -2.0)
            cameraAngleX = -2.0;

        mouseXAnterior = x;
        mouseYAnterior = y;

        glutPostRedisplay(); // Atualiza a tela
    }
}

void menu(int opcao)
{
    switch (opcao)
    {
    case 1: // resetar garra
        trilhoInferiorZ = 0.0;    // Coordenada Z do centro do trilho inferior
        tamanhoBraco = 2.0;       // Comprimento do braco da garra
        bracoX = 0.0;             // Coordenada X do braco da garra
        anguloGarra = -30.0;      // Angulo de abertura da garra
        break;
    case 2: // brilho suave
        nivelBrilho = 5.0;
        break;
    case 3: // brilho medio
        nivelBrilho = 25.0;
        break;
    case 4: // brilho concentrado
        nivelBrilho = 50.0;
        break;
    case 5: // acender/apagar lampada
        if (lampadaAcesa)
        {
            glDisable(GL_LIGHT0);
            lampadaAcesa = false;
        }
        else
        {
            glEnable(GL_LIGHT0);
            lampadaAcesa = true;
        }
        break;
    case 6: // mostrar controles
        if (larguraControles == 0)
            larguraControles = 300;
        else
            larguraControles = 0;
        
        larguraJanela = larguraCena + larguraControles;
        glutReshapeWindow(larguraJanela, alturaJanela);  // Muda o tamanho da janela
        break;
    }

    glutPostRedisplay();
}

void reshape(int largura, int altura)
{
    glViewport(0, 0, larguraCena, alturaJanela);  // Define a nova area de visualizacao
    glMatrixMode(GL_PROJECTION);        // Definir a matriz de projecao
    glLoadIdentity();                   // Limpar a matriz de projecao
    gluPerspective(45.0, (GLfloat)larguraCena / (GLfloat)alturaJanela, 1.0, 100.0); // Ajusta a perspectiva
    glMatrixMode(GL_MODELVIEW);         // Volta para a matriz de modelagem
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(larguraJanela, alturaJanela);
    glutCreateWindow("Maquina de Bichinhos");

    inicializa();

    glutDisplayFunc(display);
    //glutReshapeFunc(reshape);
    glutKeyboardFunc(teclado);
    glutMouseFunc(mouse);
    glutMotionFunc(movimentoMouse); // Adiciona rastreamento de movimento do mouse
    glutTimerFunc(1000, anima, 0);  // Inicia a animacao apos 1 segundo

    int submenuBrilho = glutCreateMenu(menu);
    glutAddMenuEntry("Suave", 2);
    glutAddMenuEntry("Medio", 3);
    glutAddMenuEntry("Concentrado", 4);

    glutCreateMenu(menu);
    glutAddMenuEntry("Resetar garra", 1);
    glutAddSubMenu("Alterar brilho", submenuBrilho);
    glutAddMenuEntry("Acender/Apagar lampada", 5);
    glutAddMenuEntry("Mostrar/Esconder controles", 6);

    glutAttachMenu(GLUT_RIGHT_BUTTON);

    glutMainLoop();

    return 0;
}
