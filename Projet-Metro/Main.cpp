#include "TrainStation.h"
#include <iostream>

int Station::prendrePassagers() {
    int pris = passagers;
    std::cout << "Station " << nom << " a " << passagers << " passagers avant le depart du train." << std::endl;
    passagers = 0;
    return pris;
}

void Station::reinitialiserEtGenererPassagers() {
    passagers = rand() % 50 + 10;  // Générer un nouveau nombre aléatoire de passagers
}


void Station::dessiner(sf::RenderWindow& window, const sf::Font& font) {
    sf::CircleShape shape(10);
    shape.setPosition(position.x - 10, position.y - 10);
    shape.setFillColor(sf::Color::Green);
    window.draw(shape);

    sf::Text text(nom + " " + std::to_string(passagers), font, 15);
    text.setFillColor(sf::Color::White);
    text.setPosition(position.x - 10, position.y - 30);
    window.draw(text);
}

void Train::ajouterStation(const Station& station) {
    parcours.push_back(station);
}

void Train::avancer(float deltaTime) {
    if (tempsAttente > 0) {
        tempsAttente -= deltaTime;
        if (tempsAttente <= 0) {
            enMouvement = true;
            if (stationActuelle < parcours.size()) {
                std::cout << "Le train quitte la station " << parcours[stationActuelle - 1].getNom() << std::endl;
            }
        }
        return;
    }

    if (enMouvement && stationActuelle < parcours.size()) {
        position.x += vitesse.x * deltaTime;
        if (position.x >= parcours[stationActuelle].getPosition().x) {
            position.x = parcours[stationActuelle].getPosition().x;
            enMouvement = false;
            std::cout << "//////" << std::endl;
            std::cout << "Le train est arrive a la station " << parcours[stationActuelle].getNom() << std::endl;

            // À la dernière station, tous les passagers descendent
            if (stationActuelle == parcours.size() - 1) {
                std::cout << "Le train depose tous les " << passagers << " passagers a la station " << parcours[stationActuelle].getNom() << std::endl;
                passagers = 0;
            }
            else { // Sinon, déposer entre 40% et 80% des passagers
                int minPassagersADeposer = passagers * 40 / 100;
                int maxPassagersADeposer = passagers * 80 / 100;
                int passagersADeposer = minPassagersADeposer + rand() % (maxPassagersADeposer - minPassagersADeposer + 1);
                passagers -= passagersADeposer;
                std::cout << "Le train depose " << passagersADeposer << " passagers a la station " << parcours[stationActuelle].getNom() << std::endl;
            }

            // Prendre les passagers de la station actuelle
            passagers += parcours[stationActuelle].prendrePassagers();
            std::cout << "Le train prend " << passagers << " passagers" << std::endl;

            // Si ce n'est pas la première station, régénérer les passagers de la station précédente
            if (stationActuelle > 0) {
                parcours[stationActuelle + 1].reinitialiserEtGenererPassagers();
            }

            tempsAttente = 5.0f;  // Attente à la station avant de partir
            if (stationActuelle < parcours.size() - 1) {
                stationActuelle++;  // Passer à la station suivante pour le prochain mouvement
            }
        }
    }
}


void Train::dessiner(sf::RenderWindow& window, const sf::Font& font) {
    sf::RectangleShape shape(sf::Vector2f(20, 10));
    shape.setPosition(position.x - 10, position.y - 5);
    shape.setFillColor(sf::Color::Red);
    window.draw(shape);

    sf::Text text("Passagers: " + std::to_string(passagers), font, 12);
    text.setFillColor(sf::Color::White);
    text.setPosition(position.x, position.y - 20);
    window.draw(text);
}

void GestionnaireDeTrain::ajouterTrain(std::unique_ptr<Train> train) {
    trains.push_back(std::move(train));
}

void GestionnaireDeTrain::mettreAJour(float deltaTime) {
    static float tempsDepuisDerniereGeneration = 0.0f;
    tempsDepuisDerniereGeneration += deltaTime;

    for (auto& train : trains) {
        train->avancer(deltaTime);
    }
}


void GestionnaireDeTrain::dessiner(sf::RenderWindow& window, const sf::Font& font) {
    for (auto& train : trains) {
        train->dessiner(window, font);
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(1200, 900), "Train Simulation");
    GestionnaireDeTrain gestionnaire;
    sf::Font font;

    if (!font.loadFromFile("C:/Program Files/SFML/font/arial.ttf")) {
        std::cerr << "Erreur de chargement de la police 'arial.ttf'" << std::endl;
        return 1;
    }

    srand(static_cast<unsigned>(time(nullptr)));

    Station paris("Paris", { 100, 300 });
    paris.reinitialiserEtGenererPassagers();
    Station lyon("Lyon", { 250, 300 });
    Station marseille("Marseille", { 400, 300 });
    Station lille("Lille", { 500, 300 });
    Station rennes("Rennes", { 700,300 });
    Station toulouse("Toulouse", { 800,300 });
    Station bruges("Bruges", { 950,300 });

    auto train1 = std::make_unique<Train>(Coordonnees(100, 300));
    train1->ajouterStation(paris);
    train1->ajouterStation(lyon);
    train1->ajouterStation(marseille);
    train1->ajouterStation(lille);
    train1->ajouterStation(rennes);
    train1->ajouterStation(toulouse);
    train1->ajouterStation(bruges);
    train1->setVitesse(Coordonnees(50, 0));

    gestionnaire.ajouterTrain(std::move(train1));

    sf::Clock clock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        float deltaTime = clock.restart().asSeconds();

        window.clear(sf::Color::Black);

        gestionnaire.mettreAJour(deltaTime);

        // Dessiner les lignes entre les stations
        sf::Vertex line[] = {
            sf::Vertex(sf::Vector2f(paris.getPosition().x, paris.getPosition().y)),
            sf::Vertex(sf::Vector2f(lyon.getPosition().x, lyon.getPosition().y)),
            sf::Vertex(sf::Vector2f(marseille.getPosition().x, marseille.getPosition().y)),
            sf::Vertex(sf::Vector2f(bruges.getPosition().x, bruges.getPosition().y)),
            sf::Vertex(sf::Vector2f(rennes.getPosition().x, rennes.getPosition().y)),
            sf::Vertex(sf::Vector2f(toulouse.getPosition().x, toulouse.getPosition().y)),
            sf::Vertex(sf::Vector2f(lille.getPosition().x, lille.getPosition().y))
        };
        window.draw(line, 7, sf::LinesStrip);

        paris.dessiner(window, font);
        lyon.dessiner(window, font);
        marseille.dessiner(window, font);
        lille.dessiner(window, font);
        toulouse.dessiner(window, font);
        rennes.dessiner(window, font);
        bruges.dessiner(window, font);

        gestionnaire.dessiner(window, font);

        window.display();
    }

    return 0;
}