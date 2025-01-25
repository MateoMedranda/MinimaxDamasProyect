#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <string>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <omp.h>
#include <limits>
#include <vector>

#include "Boton.h"
#include "Jugador.h"

using namespace std;
using namespace sf;

int indexTutorial = 1;
string gameName = Jugador::generarNombreArchivo();

Music musica;
Music sonidoGanador;
Font cipher;

const int INF = numeric_limits<int>::max();
const int NEG_INF = numeric_limits<int>::min();

int generarTurnosAleatorios()
{
    int turno;
    for(int i=0; i<2; i++)
    {
        srand(time(0));
        turno = rand() % 2 + 1;
    }

    return turno;
}

vector<vector<int>> crearTablero()
{
    return
    {
        {-1,1,-1,1,-1,1,-1,1},
        {1,-1,1,-1,1,-1,1,-1},
        {-1,1,-1,1,-1,1,-1,1},
        {0,-1,0,-1,0,-1,0,-1},
        {-1,0,-1,0,-1,0,-1,0},
        {2,-1,2,-1,2,-1,2,-1},
        {-1,2,-1,2,-1,2,-1,2},
        {2,-1,2,-1,2,-1,2,-1},
    };
}

void cambiarColorBoton(Boton &boton, RenderWindow &ventana)
{

    if (boton.isMouseOver(ventana))
    {
        boton.setBackColor(Color::Black);
        boton.setTextColor(Color::Yellow);
    }
    else
    {
        boton.setBackColor(Color(255, 255, 255, 0));
        boton.setTextColor(Color::White);
    }
}

void mostrarMensaje(RenderWindow& window, Font& font)
{
    cout << "Iniciando ventana de mensaje" << endl;

    RenderWindow mensajeWindow(VideoMode(400, 300), "Mensaje", Style::Close);
    VideoMode desktopMode = VideoMode::getDesktopMode();
    mensajeWindow.setPosition(Vector2i((desktopMode.width - mensajeWindow.getSize().x) / 2,
                                       (desktopMode.height - mensajeWindow.getSize().y) / 2));

    Texture backgroundTexture;
    backgroundTexture.loadFromFile("Texturas/usuarioLogin.png");
    RectangleShape background(Vector2f(400, 300));
    background.setTexture(&backgroundTexture);

    Text mensaje("\nDebe ingresar todos \n los jugadores antes \n de continuar", font, 20);
    mensaje.setFillColor(Color::White);
    mensaje.setStyle(Text::Bold);

    mensaje.setPosition(90, 50);

    RectangleShape aceptarBox(Vector2f(150, 40));
    aceptarBox.setPosition(125, 170);
    aceptarBox.setFillColor(Color(50, 205, 50));

    Text aceptarButton("Aceptar", font, 24);
    aceptarButton.setPosition(150, 170);
    aceptarButton.setFillColor(Color::White);

    while (mensajeWindow.isOpen())
    {
        Event mensajeEvent;
        while (mensajeWindow.pollEvent(mensajeEvent))
        {
            if (mensajeEvent.type == Event::Closed)
            {
                mensajeWindow.close();
            }
            if (mensajeEvent.type == Event::MouseButtonPressed)
            {
                if (aceptarBox.getGlobalBounds().contains(Vector2f(mensajeEvent.mouseButton.x, mensajeEvent.mouseButton.y)))
                {
                    mensajeWindow.close();
                }
            }
        }
        mensajeWindow.clear();
        mensajeWindow.draw(background);
        mensajeWindow.draw(mensaje);
        mensajeWindow.draw(aceptarBox);
        mensajeWindow.draw(aceptarButton);
        mensajeWindow.display();
    }
}

vector<string> ventanaEntradaUsuario(RenderWindow& parentWindow, Font& font, int numJugadores)
{
    int windowHeight = 200 + 70 * numJugadores;
    RenderWindow window(VideoMode(400, windowHeight), "Jugadores", Style::Close);
    VideoMode desktopMode = VideoMode::getDesktopMode();
    window.setPosition(Vector2i((desktopMode.width - window.getSize().x) / 2,
                                (desktopMode.height - window.getSize().y) / 2));

    Texture backgroundTexture;
    backgroundTexture.loadFromFile("Texturas/usuarioLogin.png");
    RectangleShape background(Vector2f(400, windowHeight));
    background.setTexture(&backgroundTexture);

    Text titulo("Ingresar Jugadores", font, 24);
    titulo.setFillColor(Color::White);
    titulo.setStyle(Text::Bold);
    titulo.setPosition(75, 20);

    vector<RectangleShape> usuarioBoxes(numJugadores);
    vector<Text> usuarioInputs(numJugadores, Text("", font, 24));
    vector<Text> usuarioLabels(numJugadores, Text("", font, 20));

    for (int i = 0; i < numJugadores; ++i)
    {
        usuarioLabels[i].setString("Jugador " + to_string(i + 1) + ":");
        usuarioLabels[i].setPosition(50, 70 + i * 70);
        usuarioLabels[i].setFillColor(Color::White);

        usuarioBoxes[i].setSize(Vector2f(300, 40));
        usuarioBoxes[i].setPosition(50, 100 + i * 70);
        usuarioBoxes[i].setFillColor(Color::White);
        usuarioBoxes[i].setOutlineColor(Color::Black);
        usuarioBoxes[i].setOutlineThickness(1);

        usuarioInputs[i].setPosition(55, 105 + i * 70);
        usuarioInputs[i].setFillColor(Color::Black);
    }

    RectangleShape continuarBox(Vector2f(150, 40));
    continuarBox.setPosition(125, 100 + numJugadores * 70 + 10);
    continuarBox.setFillColor(Color(50, 205, 50));

    Text continuarButton("Jugar", font, 24);
    continuarButton.setPosition(170, 105 + numJugadores * 70 + 10);
    continuarButton.setFillColor(Color::White);

    vector<string> usuarioStr(numJugadores, "");
    int cajaActiva = -1;

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                window.close();
                return {};
            }

            if (event.type == Event::TextEntered && cajaActiva != -1)
            {
                if ((event.text.unicode >= 'A' && event.text.unicode <= 'Z') ||
                        (event.text.unicode >= 'a' && event.text.unicode <= 'z') ||
                        (event.text.unicode >= '0' && event.text.unicode <= '9') ||
                        event.text.unicode == '\b')
                {
                    if (event.text.unicode != '\b')
                    {
                        usuarioStr[cajaActiva] += static_cast<char>(event.text.unicode);
                    }
                    else if (!usuarioStr[cajaActiva].empty())
                    {
                        usuarioStr[cajaActiva].pop_back();
                    }
                    usuarioInputs[cajaActiva].setString(usuarioStr[cajaActiva]);
                }
            }

            if (event.type == Event::MouseButtonPressed)
            {
                for (int i = 0; i < numJugadores; ++i)
                {
                    if (usuarioBoxes[i].getGlobalBounds().contains(Vector2f(event.mouseButton.x, event.mouseButton.y)))
                    {
                        cajaActiva = i;
                        break;
                    }
                }
                if (continuarBox.getGlobalBounds().contains(Vector2f(event.mouseButton.x, event.mouseButton.y)))
                {
                    bool todasCompletas = true;
                    for (int i = 0; i < numJugadores; ++i)
                    {
                        if (usuarioStr[i].empty())
                        {
                            todasCompletas = false;
                            break;
                        }
                    }

                    if (todasCompletas)
                    {
                        window.close();

                        Jugador::inicializarArchivo(gameName);

                        cout << "Nuevo juego iniciado" << endl;
                        cout << "Jugadores: ";
                        for (const auto& jugador : usuarioStr)
                        {
                            cout << jugador << " ";
                        }
                        cout << endl;

                        //Guardo el nombre del jugador
                        for (const auto& jugador : usuarioStr)
                        {
                            Jugador nuevoJugador(jugador);
                        }


                        return usuarioStr;
                    }
                    else
                    {
                        cout << "Debe completar todas las cajas de texto antes de continuar" << endl;
                        mostrarMensaje(window, font);
                    }
                }
            }
        }

        window.clear(Color::Black);
        window.draw(background);
        window.draw(titulo);
        for (int i = 0; i < numJugadores; ++i)
        {
            window.draw(usuarioLabels[i]);
            window.draw(usuarioBoxes[i]);
            window.draw(usuarioInputs[i]);
        }
        window.draw(continuarBox);
        window.draw(continuarButton);
        window.display();
    }

    return {};
}

bool haySaltosDisponiblesDamas(int x, int y, vector<vector<int>> tablero, int dx[], int dy[], int n)
{
    for (int i = 0; i < n; ++i)
    {
        int newX = x + dx[i];
        int newY = y + dy[i];

        if (newX >= 0 && newX < 8 && newY >= 0 && newY < 8 && tablero[newY][newX] != 0 &&
                tablero[newY][newX] != tablero[y][x] &&
                !(tablero[newY][newX] + 2 == tablero[y][x]) && !(tablero[newY][newX] == 2 + tablero[y][x]))
        {

            int saltoX = newX + dx[i];
            int saltoY = newY + dy[i];

            if (saltoX >= 0 && saltoX < 8 && saltoY >= 0 && saltoY < 8 && tablero[saltoY][saltoX] == 0)
            {
                return true;
            }
        }
    }
    return false;
}

void mostrarPrimerTurno(int n, Font &font, string jugador)
{
    RenderWindow ventana(VideoMode(600,338),"Primer jugador");
    Texture textura, texturaDamas;
    textura.loadFromFile("Texturas/ventanaTurno.png");
    textura.setSmooth(true);
    RectangleShape fondo;
    fondo.setSize(Vector2f(600,338));
    fondo.setTexture(&textura);

    Sprite fichaDamas;

    Text cifrados;
    cifrados.setFont(cipher);
    cifrados.setCharacterSize(70);
    cifrados.setFillColor(Color::Black);
    cifrados.setString(jugador);
    cifrados.setPosition(50,230);

    Text nombre;
    nombre.setFont(font);
    nombre.setStyle(Text::Bold);
    nombre.setString(jugador);
    nombre.setCharacterSize(70);
    nombre.setColor(Color(99, 255, 151 ));
    nombre.setPosition(50, 160);
    nombre.setStyle(Text::Underlined);

    while (ventana.isOpen())
    {
        Event evento;
        while (ventana.pollEvent(evento))
        {
            if (evento.type == Event::Closed)
            {
                ventana.close();
            }
        }
        ventana.clear();
        ventana.draw(fondo);
        fichaDamas.setPosition(50,75);
        if(n == 1)
        {
            texturaDamas.loadFromFile("Texturas/damaBlanca.png");
            fichaDamas.setTexture(texturaDamas);
        }
        else
        {
            texturaDamas.loadFromFile("Texturas/damaNegra.png");
            fichaDamas.setTexture(texturaDamas);
        }
        ventana.draw(fichaDamas);

        ventana.draw(cifrados);
        ventana.draw(nombre);
        ventana.display();
    }
}

bool quedanMovimientos(vector<vector<int>> tablero, int turno)
{

    int contBlancas = 0, contadorNegras = 0;

    for (int i = 0; i <8; i++)
    {
        for (int j = 0; j< 8; j++)
        {

            int dx[4];
            int dy[4];
            int n;

            if(tablero[i][j] == 1)
            {
                dx[0] = -1;
                dx[1] = 1;
                dy[0] = 1;
                dy[1] = 1;
                n = 2;

            }
            else if(tablero[i][j] == 2)
            {
                dx[0] = -1;
                dx[1] = 1;
                dy[0] = -1;
                dy[1] = -1;
                n = 2;

            }
            else if(tablero[i][j] == 3 || tablero[i][j] == 4)
            {
                dx[0] = -1;
                dx[1] = 1;
                dx[2] = -1;
                dx[3] = 1;
                dy[0] = -1;
                dy[1] = -1;
                dy[2] = 1;
                dy[3] = 1;
                n = 4;
            }

            for (int k = 0; k < n; ++k)
            {
                int newX = j + dx[k];
                int newY = i + dy[k];

                if (newX >= 0 && newX < 8 && newY >= 0 && newY < 8 && (tablero[newY][newX] == 0))
                {

                    if(tablero[i][j] == 1)
                    {
                        contBlancas++;
                    }
                    else if(tablero[i][j] == 2)
                    {
                        contadorNegras++;
                    }
                }

                if (newX >= 0 && newX < 8 && newY >= 0 && newY < 8 && (tablero[newY][newX] != tablero[i][j]) && (tablero[newY][newX] != 0) && !(tablero[newY][newX]+2 == tablero[i][j]) && !(tablero[newY][newX] == 2+tablero[i][j]))
                {
                    int antx = newX, anty = newY;
                    newX = newX + dx[k];
                    newY = newY + dy[k];

                    if(newX >= 0 && newX < 8 && newY >= 0 && newY < 8 && tablero[newY][newX] == 0)
                    {
                        if(tablero[i][j] == 1)
                        {
                            contBlancas++;
                        }
                        else if(tablero[i][j] == 2)
                        {
                            contadorNegras++;
                        }
                    }
                }
            }
        }
    }

    if(contBlancas == 0 && turno == 1)
    {
        return false;
    }
    else
    {
        return true;
    }

    if(contadorNegras == 0 && turno == 2)
    {
        return false;
    }
    else
    {
        return true;
    }

}

bool quedanFichasOponente(vector<vector<int>> tablero, int &jugador)
{
    int contBlancas = 0, contNegras = 0;

    for(int i = 0; i<8; i++)
    {
        for(int j = 0; j< 8; j++)
        {
            if(tablero[i][j] == 1 || tablero[i][j] == 3)
            {
                contBlancas++;
            }
            else if(tablero[i][j] == 2 || tablero[i][j] == 4)
            {
                contNegras++;
            }
        }
    }

    if((contBlancas == 0) || (contNegras == 0))
    {
        if(contBlancas == 0)
        {
            jugador = 1;
        }
        else if(contNegras == 0)
        {
            jugador = 0;
        }
        return false;
    }
    else
    {
        return true;
    }
}

void mostrarGanador(Jugador ganador, Font &font, int m, int n)
{
    RenderWindow ventana(VideoMode(900,506),"Ganador");

    musica.stop();
    sonidoGanador.openFromFile("Audios/musicaGanadorCaptura.ogg");

    sonidoGanador.play();
    sonidoGanador.setVolume(30);
    sonidoGanador.setLoop(true);

    RectangleShape fondo;
    fondo.setSize(Vector2f(900,506));

    Text texto, texto1;
    texto.setFont(font);
    texto.setCharacterSize(25);

    texto.setColor(Color::White);

    texto1.setFont(font);
    texto1.setCharacterSize(25);
    texto1.setPosition(270,400);
    texto1.setColor(Color::White);

    if(n != 2)
    {
        if(n == -1)
        {
            texto1.setString("Ya no quedan fichas oponentes");
        }
        else if(n == 3)
        {
            texto1.setString("Ganador, llegó con al menos una ficha \n al otro lado y esta llena la punta de la estrella");
        }
        else
        {
            texto1.setString("No existen movimientos posibles,\n mayor puntaje ganador");
        }
        texto.setString("Jugador: " + ganador.getUsuario() + "\nPuntaje: " + to_string(ganador.obtenerPuntos()) + "\nMovimientos: " + to_string(ganador.getMovimientos()));
    }
    else
    {
        texto.setString("EMPATE");
        texto1.setString("No existen movimientos posibles \ny ambos tienen el mismo puntaje");
    }

    Texture textura;
    texto.setPosition(270,240);
    textura.loadFromFile("Texturas/ganadorModoCaptura.png");

    textura.setSmooth(true);
    fondo.setTexture(&textura);

    while(ventana.isOpen())
    {
        Event evento;
        while (ventana.pollEvent(evento))
        {
            if (evento.type == Event::Closed)
            {
                sonidoGanador.stop();
                musica.play();
                ventana.close();
            }
        }
        ventana.clear();
        ventana.draw(fondo);
        ventana.draw(texto);
        ventana.draw(texto1);
        ventana.display();
    }
}

void jugarDamas(Font &font, vector<string> jugadores)
{
    vector<Jugador> listaJugadores;
    for (const auto& nombre : jugadores)
    {
        listaJugadores.emplace_back(nombre);
    }

    SoundBuffer moverBuffer, comerBuffer,damaBuffer;
    Sound sonidoMover, sonidoComer, sonidoDama;

    Text texto[2], puntajesTexto[2], movimientosTexto[2];

    for(int i = 0; i<2; i++)
    {
        texto[i].setCharacterSize(30);
        texto[i].setFont(font);
        texto[i].setStyle(Text::Underlined);
        texto[i].setString(jugadores[i]);
        texto[i].setPosition(50+(i*944), 226);
        texto[i].setColor(Color::Cyan);

        puntajesTexto[i].setCharacterSize(30);
        puntajesTexto[i].setFont(font);
        puntajesTexto[i].setPosition(50 + (i * 944), 266); // Posición para el puntaje
        puntajesTexto[i].setFillColor(Color::White);
        puntajesTexto[i].setString("Puntaje: \n0");

        movimientosTexto[i].setCharacterSize(25);
        movimientosTexto[i].setFont(font);
        movimientosTexto[i].setPosition(50 + (i * 944), 344);
        movimientosTexto[i].setFillColor(Color::White);
        movimientosTexto[i].setString("Movimientos: \n0");

    }

    moverBuffer.loadFromFile("Audios/moverPieza.ogg");
    sonidoMover.setBuffer(moverBuffer);
    sonidoMover.setVolume(100);
    comerBuffer.loadFromFile("Audios/comerPieza.ogg");
    sonidoComer.setBuffer(comerBuffer);
    sonidoComer.setVolume(100);
    damaBuffer.loadFromFile("Audios/obtenerDama.wav");
    sonidoDama.setBuffer(damaBuffer);
    sonidoDama.setVolume(15);

    RenderWindow Damas(VideoMode(1200,675),"Damas");
    Damas.setPosition(Vector2i(0,0));

    vector<vector<int>> tablero = crearTablero();

    RectangleShape fondo;
    Texture textura, textura1, textura2, textura3, textura4;
    fondo.setSize(Vector2f(1200,675));
    textura.loadFromFile("Texturas/tablero_damas.png");
    textura.setSmooth(true);
    textura1.loadFromFile("Texturas/damaBaseBlanca.png");
    textura2.loadFromFile("Texturas/damaBaseNegra.png");
    textura3.loadFromFile("Texturas/damaBlanca.png");
    textura4.loadFromFile("Texturas/damaNegra.png");
    fondo.setTexture(&textura);

    Sprite fichaBlanca, fichaNegra, damaBlanca, damaNegra;
    fichaBlanca.setTexture(textura1);
    fichaNegra.setTexture(textura2);
    damaBlanca.setTexture(textura3);
    damaNegra.setTexture(textura4);

    CircleShape movimiento(30.0f); // Tamaño del círculo para mostrar los movimientos
    movimiento.setFillColor(Color(0, 255, 0, 100)); // Color verde transparente
    movimiento.setOutlineThickness(6.f);
    movimiento.setOutlineColor(sf::Color(250, 150, 100));

    const float offsetX = 260.0f;
    const float offsetY = 0.0f;
    const float tamanoCasilla = 85.0f;

    bool fichaSeleccionada = false;
    int fichaSeleccionadaX = -1;
    int fichaSeleccionadaY = -1;
    int turno = generarTurnosAleatorios();
    //int puntajes[2] = {0, 0}; // Puntajes de los jugadores.
    bool salto = false;
    int xSalto = -1;
    int ySalto = -1;
    int ganador;
    int puntajeGanador;

    mostrarPrimerTurno(turno,font,jugadores[turno-1]);
    vector<int> puntajes(jugadores.size(), 0);
    vector<int> movimientosLista(jugadores.size(), 0);

    while(Damas.isOpen())
    {
        Event aevent;
        while(Damas.pollEvent(aevent))
        {

            if(!quedanFichasOponente(tablero,ganador))
            {
                cout << "se quedo sin fichas " << ganador <<  endl;
                puntajeGanador = -1;
                for (size_t i = 0; i < listaJugadores.size(); ++i)
                {
                    listaJugadores[i].actualizarPuntos(puntajes[i]);
                    listaJugadores[i].actualizarMovimientos(movimientosLista[i]);
                }
                Jugador::determinarEstado(puntajes, listaJugadores);
                Jugador::guardarJugadoresEnArchivo(listaJugadores);

                mostrarGanador(listaJugadores[ganador],font, 1, puntajeGanador);
                Damas.close();
                break;
            }

            if(!quedanMovimientos(tablero, turno))
            {
                cout << "se quedo sin movimientos" << endl;
                for (size_t i = 0; i < listaJugadores.size(); ++i)
                {
                    listaJugadores[i].actualizarPuntos(puntajes[i]);
                    listaJugadores[i].actualizarMovimientos(movimientosLista[i]);
                }
                Jugador::determinarEstado(puntajes, listaJugadores);

                if(puntajes[0] > puntajes [1])
                {
                    puntajeGanador = 0;
                    mostrarGanador(listaJugadores[puntajeGanador], font,1, puntajeGanador);
                }
                else if(puntajes[0] < puntajes[1])
                {
                    puntajeGanador = 1;
                    mostrarGanador(listaJugadores[puntajeGanador], font,1, puntajeGanador);
                }
                else
                {
                    puntajeGanador = 2;
                    mostrarGanador(listaJugadores[0], font,1, puntajeGanador);
                }
                Jugador::guardarJugadoresEnArchivo(listaJugadores);

                Damas.close();
                break;
            }

            switch(aevent.type)
            {
            case Event::Closed:
            {
                Jugador::determinarEstado(puntajes, listaJugadores);
                Jugador::guardarJugadoresEnArchivo(listaJugadores);
                Damas.close();
            }
            break;

            case Event::MouseButtonPressed:
            {

                int temp;
                if (aevent.mouseButton.button == Mouse::Left)
                {
                    float x = Mouse::getPosition(Damas).x;
                    float y = Mouse::getPosition(Damas).y;

                    cout << x << endl;
                    cout << y << endl;

                    int i = (y-offsetY)/tamanoCasilla;
                    int j = (x-offsetX)/tamanoCasilla;

                    cout << "i: " << i << "j: " << j << endl;


                    if((i>=0) && (i<8) && (j>=0) && (j<8))
                    {
                        if(((tablero[i][j] == 1) || (tablero[i][j] == 2) || (tablero[i][j] == 3) || (tablero[i][j] == 4)) && (tablero[i][j] == turno || tablero[i][j] == turno+2))
                        {

                            if (fichaSeleccionada)
                            {
                                fichaSeleccionada = false; // Deselecciona la ficha
                            }
                            else
                            {
                                fichaSeleccionada = true; // Selecciona una ficha
                                fichaSeleccionadaX = j;
                                fichaSeleccionadaY = i;
                            }

                            if(salto)
                            {
                                fichaSeleccionada = true;
                                fichaSeleccionadaX = xSalto;
                                fichaSeleccionadaY = ySalto;
                            }

                        }
                        else if(tablero[i][j] == 0 && fichaSeleccionada)
                        {

                            int dx[4];
                            int dy[4];
                            int n;

                            if(tablero[fichaSeleccionadaY][fichaSeleccionadaX] == 1)
                            {

                                dx[0] = -1;
                                dx[1] = 1;
                                dy[0] = 1;
                                dy[1] = 1;
                                n = 2;

                            }
                            else if(tablero[fichaSeleccionadaY][fichaSeleccionadaX] == 2)
                            {

                                dx[0] = -1;
                                dx[1] = 1;
                                dy[0] = -1;
                                dy[1] = -1;
                                n = 2;

                            }
                            else if(tablero[fichaSeleccionadaY][fichaSeleccionadaX] == 3 || tablero[fichaSeleccionadaY][fichaSeleccionadaX] == 4)
                            {
                                dx[0] = -1;
                                dx[1] = 1;
                                dx[2] = -1;
                                dx[3] = 1;
                                dy[0] = -1;
                                dy[1] = -1;
                                dy[2] = 1;
                                dy[3] = 1;
                                n = 4;
                            }

                            for (int k = 0; k < n; ++k)
                            {
                                int newX = fichaSeleccionadaX + dx[k];
                                int newY = fichaSeleccionadaY + dy[k];

                                if (newX >= 0 && newX < 8 && newY >= 0 && newY < 8 && (tablero[newY][newX] == 0) && !salto)
                                {
                                    cout << "fichax: " << fichaSeleccionadaX << "fichaY: " << fichaSeleccionadaY << endl;
                                    cout << "x: " << newX << "y: " << newY << endl;
                                    if(newX == j && newY == i)
                                    {
                                        temp = tablero[fichaSeleccionadaY][fichaSeleccionadaX];
                                        tablero[fichaSeleccionadaY][fichaSeleccionadaX] = 0;
                                        tablero[newY][newX] = temp;
                                        sonidoMover.play();
                                        if(turno == 1)
                                        {
                                            movimientosLista[0]+=1;
                                            turno = 2;
                                        }
                                        else
                                        {
                                            movimientosLista[1]+=1;
                                            turno = 1;
                                        }

                                        if(newY == 0 && temp == 2)
                                        {
                                            tablero[newY][newX] = 4;
                                            sonidoDama.play();
                                        }
                                        if(newY == 7 && temp == 1)
                                        {
                                            tablero[newY][newX] = 3;
                                            sonidoDama.play();
                                        }
                                    }
                                }

                                if (newX >= 0 && newX < 8 && newY >= 0 && newY < 8 && (tablero[newY][newX] != tablero[fichaSeleccionadaY][fichaSeleccionadaX]) && (tablero[newY][newX] != 0) && !(tablero[newY][newX]+2 == tablero[fichaSeleccionadaY][fichaSeleccionadaX]) && !(tablero[newY][newX] == 2+tablero[fichaSeleccionadaY][fichaSeleccionadaX]))
                                {
                                    int antx = newX, anty = newY;
                                    newX = newX + dx[k];
                                    newY = newY + dy[k];

                                    if(newX == j && newY == i && tablero[newY][newX] == 0)
                                    {
                                        temp = tablero[fichaSeleccionadaY][fichaSeleccionadaX];
                                        if (temp == 3 || temp == 4)
                                        {
                                            if (tablero[anty][antx] == 3 || tablero[anty][antx] == 4)
                                            {
                                                puntajes[turno - 1] += 1; // REina captura reina
                                            }
                                            else
                                            {
                                                puntajes[turno - 1] += 1; // reina captura ficha
                                            }
                                        }
                                        else if (tablero[anty][antx] == 3 || tablero[anty][antx] == 4)
                                        {
                                            puntajes[turno - 1] += 5; // ficha captura reina
                                        }
                                        else
                                        {
                                            puntajes[turno - 1] += 1; // ficha captura  ficha
                                        }

                                        tablero[fichaSeleccionadaY][fichaSeleccionadaX] = 0;
                                        tablero[newY][newX] = temp;
                                        tablero[anty][antx] = 0;
                                        sonidoMover.play();
                                        sonidoComer.play();

                                        if (haySaltosDisponiblesDamas(newX, newY, tablero, dx, dy, n))
                                        {

                                            salto = true;
                                            xSalto = newX;
                                            ySalto = newY;
                                        }
                                        else
                                        {

                                            salto = false;
                                            if(turno == 1)
                                            {
                                                movimientosLista[0]+=1;
                                                turno = 2;
                                            }
                                            else
                                            {
                                                movimientosLista[1]+=1;
                                                turno = 1;
                                            }

                                        }


                                        if(newY == 0 && temp == 2)
                                        {
                                            tablero[newY][newX] = 4;
                                            sonidoDama.play();
                                        }
                                        if(newY == 7 && temp == 1)
                                        {
                                            tablero[newY][newX] = 3;
                                            sonidoDama.play();
                                        }
                                    }

                                }
                            }
                            fichaSeleccionada = false;
                        }
                    }

                }

            }
            break;
            }
        }

        puntajesTexto[0].setString("Puntaje: \n" + to_string(puntajes[0]));
        puntajesTexto[1].setString("Puntaje: \n" + to_string(puntajes[1]));
        movimientosTexto[0].setString("Movimientos: \n" + to_string(movimientosLista[0]));
        movimientosTexto[1].setString("Movimientos: \n" + to_string(movimientosLista[1]));

        for (size_t i = 0; i < listaJugadores.size(); ++i)
        {
            listaJugadores[i].actualizarPuntos(puntajes[i]);
        }

        for (size_t i = 0; i < listaJugadores.size(); ++i)
        {
            listaJugadores[i].actualizarMovimientos(movimientosLista[i]);
        }

        Damas.clear();
        Damas.draw(fondo);
        for(int i = 0; i<2; i++)
        {
            Damas.draw(texto[i]);
            Damas.draw(puntajesTexto[i]);
            Damas.draw(movimientosTexto[i]);
        }

        for(int i = 0; i < 8; i++)
        {
            for(int j = 0; j < 8; j++)
            {
                float x = offsetX + j * tamanoCasilla;
                float y = offsetY + i * tamanoCasilla;

                if(tablero[i][j] == 1)
                {
                    fichaBlanca.setPosition(x, y);
                    Damas.draw(fichaBlanca);
                }
                if(tablero[i][j] == 2)
                {
                    fichaNegra.setPosition(x, y);
                    Damas.draw(fichaNegra);
                }
                if(tablero[i][j] == 3)
                {
                    damaBlanca.setPosition(x,y);
                    Damas.draw(damaBlanca);
                }
                if(tablero[i][j] == 4)
                {
                    damaNegra.setPosition(x,y);
                    Damas.draw(damaNegra);
                }
            }
        }

        if (fichaSeleccionada || salto)
        {
            int dx[4];
            int dy[4];
            int n;

            if(tablero[fichaSeleccionadaY][fichaSeleccionadaX] == 1)
            {

                dx[0] = -1;
                dx[1] = 1;
                dy[0] = 1;
                dy[1] = 1;
                n = 2;

            }
            else if(tablero[fichaSeleccionadaY][fichaSeleccionadaX] == 2)
            {

                dx[0] = -1;
                dx[1] = 1;
                dy[0] = -1;
                dy[1] = -1;
                n = 2;

            }
            else if (tablero[fichaSeleccionadaY][fichaSeleccionadaX] == 3 || tablero[fichaSeleccionadaY][fichaSeleccionadaX] == 4 )
            {
                dx[0] = -1;
                dx[1] = 1;
                dx[2] = -1;
                dx[3] = 1;
                dy[0] = -1;
                dy[1] = -1;
                dy[2] = 1;
                dy[3] = 1;
                n = 4;
            }

            if(salto)
            {
                fichaSeleccionada = true;
                fichaSeleccionadaX = xSalto;
                fichaSeleccionadaY = ySalto;
            }

            for (int i = 0; i < n; ++i)
            {
                int newX = fichaSeleccionadaX + dx[i];
                int newY = fichaSeleccionadaY + dy[i];

                if (newX >= 0 && newX < 8 && newY >= 0 && newY < 8 && (tablero[newY][newX] == 0) && !salto)
                {
                    movimiento.setPosition(offsetX + newX*tamanoCasilla + (tamanoCasilla/2 - movimiento.getRadius()),offsetY + newY*tamanoCasilla + (tamanoCasilla/ 2 - movimiento.getRadius()));
                    Damas.draw(movimiento);
                }

                if (newX >= 0 && newX < 8 && newY >= 0 && newY < 8 && (tablero[newY][newX] != tablero[fichaSeleccionadaY][fichaSeleccionadaX]) && (tablero[newY][newX] != 0) && !(tablero[newY][newX]+2 == tablero[fichaSeleccionadaY][fichaSeleccionadaX])&& !(tablero[newY][newX] == 2+ tablero[fichaSeleccionadaY][fichaSeleccionadaX]))
                {
                    newX = newX+ dx[i];
                    newY = newY + dy[i];
                    if(tablero[newY][newX] == 0 && newX >= 0 && newX < 8 && newY >= 0 && newY < 8)
                    {
                        movimiento.setPosition(offsetX + newX*tamanoCasilla + (tamanoCasilla/2 - movimiento.getRadius()),offsetY + newY*tamanoCasilla + (tamanoCasilla/ 2 - movimiento.getRadius()));
                        Damas.draw(movimiento);
                    }
                }
            }

        }
        Damas.display();
    }
}

//Aqui va todo el algoritmo para minimax

int evaluarTablero(vector<vector<int>> tablero){

    int valor = 0;

    for(int i=0; i<8;i++){
        for(int j=0;j<8;j++){
            if(tablero[i][j] == 1){
                valor += 10;
            }else if (tablero[i][j] == 2){
                valor -= 10;
            }else if (tablero[i][j] == 3){
                valor +=20;
            }else if (tablero[i][j] == 4){
                valor -=20;
            }
        }
    }

    return valor;
}

void obtenerSaltos(vector<vector<vector<int>>> &movimientos, vector<vector<vector<int>>> &saltos, vector<vector<int>> movimiento, int x, int y, int dx[], int dy[], int n){

    if(!haySaltosDisponiblesDamas(x,y,movimiento,dx,dy,n)){
        movimientos.push_back(movimiento);
        return;
    }

    vector<vector<int>> tablero = movimiento;

    for (int i = 0; i < n; ++i)
    {
        int newX = x + dx[i];
        int newY = y + dy[i];
        int temp;

        if (newX >= 0 && newX < 8 && newY >= 0 && newY < 8 && (tablero[newY][newX] != tablero[y][x]) && (tablero[newY][newX] != 0) && !(tablero[newY][newX]+2 == tablero[y][x]) && !(tablero[newY][newX] == 2+tablero[y][x]))
        {
            int antx = newX, anty = newY;
            newX = newX + dx[i];
            newY = newY + dy[i];

            if(newX >= 0 && newX < 8 && newY >= 0 && newY < 8 && tablero[newY][newX] == 0)
            {

                temp = tablero[y][x];
                tablero[y][x] = 0;
                tablero[newY][newX] = temp;
                tablero[anty][antx] = 0;

                if(newY == 0 && temp == 2) tablero[newY][newX] = 4;
                if(newY == 7 && temp == 1) tablero[newY][newX] = 3;

                saltos.push_back(tablero);
                obtenerSaltos(movimientos, saltos,tablero,newX,newY,dx,dy,n);

                tablero = movimiento;
            }

        }
    }
    return;
}

vector<vector<vector<int>>> obtenerMovimientosPosibles(vector<vector<int>> estado, int jugador){
    vector<vector<vector<int>>> movimientos;
    vector<vector<vector<int>>> saltos;

    vector<vector<int>> movimiento = estado;

    for(int i = 0; i<8; i++)
    {
        for(int j = 0; j<8; j++)
        {

            int dx[4];
            int dy[4];
            int n;

            if(movimiento[i][j] == 1)
            {

                dx[0] = -1;
                dx[1] = 1;
                dy[0] = 1;
                dy[1] = 1;
                n = 2;

            }
            else if(movimiento[i][j] == 2)
            {

                dx[0] = -1;
                dx[1] = 1;
                dy[0] = -1;
                dy[1] = -1;
                n = 2;

            }
            else if(movimiento[i][j] == 3 || movimiento[i][j] == 4)
            {

                dx[0] = -1;
                dx[1] = 1;
                dx[2] = -1;
                dx[3] = 1;
                dy[0] = -1;
                dy[1] = -1;
                dy[2] = 1;
                dy[3] = 1;
                n = 4;
            }

            bool salto = false;
            int newX;
            int newY;

            if(movimiento[i][j]==jugador || movimiento[i][j] == jugador+2)
            {
                for(int k=0; k<n; k++)
                {


                    int newX = j + dx[k];
                    int newY = i + dy[k];

                    if (newX >= 0 && newX < 8 && newY >= 0 && newY < 8 && (movimiento[newY][newX] == 0) && !salto)
                    {
                        int temp = movimiento[i][j];
                        movimiento[i][j] = 0;
                        movimiento[newY][newX] = temp;
                        movimientos.push_back(movimiento);
                    }

                    movimiento = estado;
                }

                if(haySaltosDisponiblesDamas(j,i,movimiento,dx,dy,n)){
                    obtenerSaltos(movimientos,saltos,movimiento,j,i,dx,dy,n);

                }
            }


        }

    }

    return movimientos;
}

int minimax(vector<vector<int>> tablero, int profundidad, int jugador, bool esMax){
    if(!quedanMovimientos(tablero,jugador) || !quedanFichasOponente(tablero, jugador) || profundidad == 0){
        return evaluarTablero(tablero);
    }

    if (esMax) {
        int maxEval = NEG_INF;
        for (const auto& movimiento : obtenerMovimientosPosibles(tablero, jugador)) {
            int eval = minimax(movimiento, profundidad-1,jugador+1, false);
            maxEval = max(maxEval, eval);
        }
        return maxEval;

    }else {
        int minEval = INF;
        for (const auto& movimiento : obtenerMovimientosPosibles(tablero,jugador)) {
            int eval = minimax(movimiento, profundidad - 1,jugador-1, true);
            minEval = min(minEval, eval);
        }
        return minEval;
    }


}

 vector<vector<int>> encontrarMejorMovimiento(vector<vector<int>> tablero, int profundidad, int jugador){
    int mejorValor = NEG_INF;
    vector<vector<int>> mejorMovimiento;

    for (const auto& movimiento : obtenerMovimientosPosibles(tablero, jugador)) {

        int valorMovimiento = minimax(movimiento, profundidad - 1,jugador+1, false);

        if (valorMovimiento > mejorValor) {
            mejorValor = valorMovimiento;
            mejorMovimiento = movimiento;
        }
    }

    return mejorMovimiento;
 }

//Aqui termina el algoritmo para minimax


void abrirJugar(RenderWindow &Jugar, Font &font, Boton modosJuego[])
{
    RectangleShape fondo;
    Texture texturaModos;
    fondo.setSize(Vector2f(1200,675));
    texturaModos.loadFromFile("Texturas/fondoModosDeJuego.png");
    texturaModos.setSmooth(true);
    fondo.setTexture(&texturaModos);

    Event aevent;
    while(Jugar.pollEvent(aevent))
    {
        switch(aevent.type)
        {
        case Event::Closed:
            Jugar.close();
            break;

        case Event::MouseMoved:
            for (int i = 0; i < 6; ++i)
            {
                cambiarColorBoton(modosJuego[i], Jugar);
            }
            break;

        case Event::MouseButtonPressed:
        {
            bool cerrado = false;

            if(modosJuego[5].isMouseOver(Jugar))
            {
                //Jugar.close();
                vector<string> jugadores = ventanaEntradaUsuario(Jugar, font, 2);
                if (!jugadores.empty())
                {
                    jugarDamas(font, jugadores);
                    cerrado = true;
                }

            }
            if(cerrado)
            {
                Jugar.create(VideoMode(1200,675), "Jugar", Style::Default);
                Jugar.setPosition(Vector2i(0,0));
            }

        }

        break;
        }
    }

    Jugar.clear();
    Jugar.draw(fondo);
    for(int i = 0; i < 6; i++)
    {
        modosJuego[i].drawTo(Jugar);
    }
}

void abrirTutorial(RenderWindow &Opcion, Font &font, Boton &siguiente, Boton &anterior)
{
    RectangleShape fondo;
    Texture texturaTutorial;
    string tutorial = "tutorial_";
    stringstream ss;
    ss << tutorial << indexTutorial;
    string archivo = ss.str();
    Event aevent;

    fondo.setSize(Vector2f(1200,675));
    texturaTutorial.loadFromFile("Texturas/" + archivo+ ".png");
    texturaTutorial.setSmooth(true);
    fondo.setTexture(&texturaTutorial);

    while(Opcion.pollEvent(aevent))
    {
        if(aevent.type == Event::Closed)
        {
            Opcion.close();
        }
        if(aevent.type == Event::KeyPressed)
        {
            if(aevent.key.code == Keyboard::Escape)
            {
                Opcion.close();
            }
        }

        switch(aevent.type)
        {
        case Event::Closed:
        {
            Opcion.close();
        }
        break;
        case Event::KeyPressed:
        {
            if(aevent.key.code == Keyboard::Escape)
            {
                Opcion.close();
            }
        }
        break;
        case Event::MouseMoved:
        {
            if(siguiente.isMouseOver(Opcion))
            {
                siguiente.setBackColor(Color::Black);
            }
            else
            {
                siguiente.setBackColor(Color(255, 255, 255, 0));
            }
            if(anterior.isMouseOver(Opcion))
            {
                anterior.setBackColor(Color::Black);
            }
            else
            {
                anterior.setBackColor(Color(255, 255, 255, 0));
            }
        }
        break;
        case Event::MouseButtonPressed:
        {
            if(siguiente.isMouseOver(Opcion) && (indexTutorial != 12))
            {
                indexTutorial +=1;
            }
            if(anterior.isMouseOver(Opcion) && (indexTutorial != 1))
            {
                indexTutorial -=1;
            }
        }
        break;
        }

    }
    Opcion.clear();
    Opcion.draw(fondo);
    anterior.drawTo(Opcion);
    siguiente.drawTo(Opcion);
}

void iniciarJuego()
{
    RenderWindow MENU(VideoMode(1200,675), "Menu Principal", Style::Default);
    cipher.loadFromFile("Fuentes/CipherFontA.ttf");

    MENU.setPosition(Vector2i(0,0));

    Texture soundOnTextura;
    soundOnTextura.loadFromFile("Texturas/sound_on.png");

    Texture soundOffTextura;
    soundOffTextura.loadFromFile("Texturas/sound_off.png");

    Sprite soundButton;
    soundButton.setTexture(soundOnTextura);
    soundButton.setPosition(25, 25);
    soundButton.setScale(0.5f, 0.5f);

    musica.openFromFile("Audios/musicaMenu.ogg");


    RectangleShape fondo, fondoC;

    fondo.setSize(Vector2f(1200,675));
    fondoC.setSize(Vector2f(1200,675));
    Texture texturaPrincipal, texturaCreditos;
    texturaCreditos.loadFromFile("Texturas/Creditos.png");

    texturaPrincipal.loadFromFile("Texturas/fondoMenuPrincipal.png");
    texturaPrincipal.setSmooth(true);
    texturaCreditos.setSmooth(true);
    fondo.setTexture(&texturaPrincipal);
    fondoC.setTexture(&texturaCreditos);

    Font font;
    font.loadFromFile("Fuentes/LemonShakeShake.ttf");

    Boton botones[4];
    int posicion = 100;

    string textoBotonesMenu[] = {"Jugar", "Tutorial", "Creditos", "Salir"};

    for (int i = 0; i<4; i++)
    {
        botones[i] = Boton(textoBotonesMenu[i], {325,80},70,Color(255, 255, 255, 10),Color::White);
        botones[i].setPosition({225, posicion + i*100});
        botones[i].setFont(font);

    }

    Boton siguiente = Boton("Siguiente", {310,60},45,Color(255, 255, 255, 0),Color::White);
    siguiente.setPosition({850, 600});
    siguiente.setFont(font);

    Boton anterior = Boton("Anterior", {310,60},45,Color(255, 255, 255, 0),Color::White);
    anterior.setPosition({850, 540});
    anterior.setFont(font);

    posicion = 125;
    Boton modosJuego[6];
    string textoBotonesJugar[] = {"2 Jugadores", "3 Jugadores", "4 Jugadores", "5 Jugadores", "6 Jugadores", "2 Jugadores"};

    for (int i = 0; i<5; i++)
    {
        modosJuego[i] = Boton(textoBotonesJugar[i], {325,55},35,Color(255, 255, 255, 0),Color::White);
        modosJuego[i].setPosition({100,posicion + i*55});
        modosJuego[i].setFont(font);
    }

    modosJuego[5] = Boton(textoBotonesJugar[5], {325,55},40,Color(255, 255, 255, 0),Color::White);

    modosJuego[5].setPosition({100,560});
    modosJuego[5].setFont(font);

    Boton botonExtra = Boton("", {325,200}, 0, Color(0,0,0,0), Color::White);
    botonExtra.setPosition(Vector2f(800, 40));
    Text cifrados;
    cifrados.setFont(cipher);
    cifrados.setCharacterSize(60);
    cifrados.setFillColor(Color(205, 231, 209 ));
    cifrados.setStyle(Text::Bold);
    cifrados.setString("En el titulo hay un boton");
    cifrados.setPosition(50,600);

    bool soundOn = true;
    musica.play();
    musica.setPlayingOffset(seconds(10.f));
    musica.setVolume(30);
    musica.setLoop(true);
    while(MENU.isOpen())
    {
        Event evento;
        while(MENU.pollEvent(evento))
        {

            switch(evento.type)
            {
            case Event::Closed:
            {
                MENU.close();
            }
            break;
            case Event::MouseMoved:
                for (int i = 0; i < 4; ++i)
                {
                    cambiarColorBoton(botones[i], MENU);
                }
                break;
            case Event::MouseButtonPressed:
            {

                if(botonExtra.isMouseOver(MENU))
                {
                    musica.stop();
                    musica.openFromFile("Audios/musicaExtra.wav");
                    texturaPrincipal.loadFromFile("Texturas/fondoMenuExtra.png");
                    musica.play();
                }

                if(botones[0].isMouseOver(MENU) || botones[1].isMouseOver(MENU) || botones[2].isMouseOver(MENU) || botones[3].isMouseOver(MENU))
                {

                    if(botones[0].isMouseOver(MENU))
                    {
                        MENU.close();
                        RenderWindow Jugar(VideoMode(1200,675),"Damas Chinas");
                        Jugar.setPosition(Vector2i(0,0));
                        while(Jugar.isOpen())
                        {
                            abrirJugar(Jugar, font, modosJuego);
                            Jugar.display();
                        }
                    }

                    if(botones[1].isMouseOver(MENU))
                    {
                        MENU.close();
                        RenderWindow Opcion(VideoMode(1200,675), "¿Como Jugar?");
                        Opcion.setPosition(Vector2i(0,0));
                        while(Opcion.isOpen())
                        {
                            abrirTutorial(Opcion, font, siguiente, anterior);
                            Opcion.display();
                        }
                    }

                    if(botones[2].isMouseOver(MENU))
                    {
                        MENU.close();
                        RenderWindow Creditos(VideoMode(1200,675),"Creditos");
                        Creditos.setPosition(Vector2i(0,0));
                        while(Creditos.isOpen())
                        {
                            Event aevent;
                            while(Creditos.pollEvent(aevent))
                            {
                                if(aevent.type == Event::Closed)
                                {
                                    Creditos.close();
                                }
                                if(aevent.type == Event::KeyPressed)
                                {
                                    if(aevent.key.code == Keyboard::Escape)
                                    {
                                        Creditos.close();
                                    }
                                }
                            }
                            Creditos.clear();
                            Creditos.draw(fondoC);
                            Creditos.display();
                        }
                    }
                    MENU.create(VideoMode(1200,675), "Menu Principal", Style::Default);
                    MENU.setPosition(Vector2i(0,0));

                    if(botones[3].isMouseOver(MENU))
                    {
                        MENU.close();
                    }

                }
                if (evento.mouseButton.button == Mouse::Left)
                {
                    Vector2i mousePos = Mouse::getPosition(MENU);
                    if (soundButton.getGlobalBounds().contains(mousePos.x, mousePos.y))
                    {
                        soundOn = !soundOn;

                        if (soundOn)
                        {
                            soundButton.setTexture(soundOnTextura);
                            musica.play(); // Reanuda la música
                        }
                        else
                        {
                            soundButton.setTexture(soundOffTextura);
                            musica.pause(); // Pausa la música
                        }
                    }
                }
            }
            break;
            }
        }
        MENU.clear();
        MENU.draw(fondo);
        MENU.draw(soundButton);
        MENU.draw(cifrados);
        for(int i = 0; i<4; i++)
        {
            botones[i].drawTo(MENU);
        }
        botonExtra.drawTo(MENU);
        MENU.display();
    }
}

int main()
{
    iniciarJuego();
    return 0;
}
