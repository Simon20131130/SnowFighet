#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iostream>

#include "Character.h"
#include "Snowball.h"
#include "Enemy.h"
#include "RangedEnemy.h"
#include "Fireball.h"
#include "HealthPickup.h"
#include "WorldObject.h"

// ======================================================
// WAVE PARAMÉTEREK
// ======================================================
static int GetEnemiesToSpawnForWave(int wave)
{
    return 8 + wave * 4;
}

static float GetSpawnDelayForWave(int wave)
{
    float d = 0.45f - (wave * 0.01f);
    if (d < 0.18f) d = 0.18f;
    return d;
}

static float GetRestTimeBetweenWaves()
{
    return 2.0f;
}
enum class GameState
{
    Home,
    Playing,
    GameOver
};
struct Snowflake
{
    sf::CircleShape shape;
    float speed;
};


GameState gameState = GameState::Home;

int main()
{
    std::vector<Snowflake> snowflakes;
    const int SNOW_COUNT = 150;

    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // ======================================================
    // VERZIÓ
    // ======================================================
    const std::string VERSION = "v1.0.0";

    // ======================================================
    // WORLD MÉRET
    // ======================================================
    const float WORLD_WIDTH = 3000.f;
    const float WORLD_HEIGHT = 2000.f;

    sf::FloatRect worldBounds(
        -WORLD_WIDTH / 2.f,
        -WORLD_HEIGHT / 2.f,
        WORLD_WIDTH,
        WORLD_HEIGHT
    );

    // ======================================================
    // FULLSCREEN ABLAK
    // ======================================================
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(
        desktop,
        "SnowFighter",
        sf::Style::Fullscreen
    );
    window.setFramerateLimit(0);

    for (int i = 0; i < SNOW_COUNT; i++)
    {
        Snowflake s;
        s.shape.setRadius(static_cast<float>(2 + rand() % 3));
        s.shape.setFillColor(sf::Color(255, 255, 255, 180));
        s.shape.setPosition(
            static_cast<float>(rand() % desktop.width),
            static_cast<float>(rand() % desktop.height)
        );

        s.speed = 40.f + static_cast<float>(rand() % 60);
        snowflakes.push_back(s);
    }
    // ======================================================
    // KAMERA + HUD VIEW
    // ======================================================
    sf::View camera;
    camera.setSize(
        static_cast<float>(desktop.width),
        static_cast<float>(desktop.height)
    );
    camera.setCenter(0.f, 0.f);

    sf::View hudView(sf::FloatRect(
        0.f, 0.f,
        static_cast<float>(desktop.width),
        static_cast<float>(desktop.height)
    ));

    // ======================================================
    // PADLÓ
    // ======================================================
    sf::Texture snowTexture;
    if (!snowTexture.loadFromFile("resources/snow.png"))
        return -1;

    snowTexture.setRepeated(true);

    sf::RectangleShape floor;
    floor.setSize({ WORLD_WIDTH, WORLD_HEIGHT });
    floor.setOrigin(WORLD_WIDTH / 2.f, WORLD_HEIGHT / 2.f);
    floor.setPosition(0.f, 0.f);
    floor.setTexture(&snowTexture);
    floor.setTextureRect(
        sf::IntRect(
            0, 0,
            static_cast<int>(WORLD_WIDTH / 2),
            static_cast<int>(WORLD_HEIGHT / 2)
        )
    );

    Enemy::SetWorldBounds(worldBounds);
    RangedEnemy::SetWorldBounds(worldBounds);
   


    // ======================================================
    // JÁTÉKOS
    // ======================================================
    Character player;

    // ======================================================
    // ENTITYK
    // ======================================================
    std::vector<Snowball> snowballs;
    std::vector<Enemy> enemies;
    std::vector<RangedEnemy> rangedEnemies;
    std::vector<Fireball> fireballs;
    std::vector<HealthPickup> healthPickups;
    std::vector<WorldObject> worldObjects;

    // ======================================================
    // WORLD OBJECTEK
    // ======================================================
    for (int i = 0; i < 30; i++)
    {
        sf::Vector2f pos = {
            worldBounds.left + static_cast<float>(rand()) / RAND_MAX * worldBounds.width,
            worldBounds.top + static_cast<float>(rand()) / RAND_MAX * worldBounds.height
        };
        worldObjects.emplace_back(WorldObject::Type::Tree, pos);
    }

    for (int i = 0; i < 20; i++)
    {
        sf::Vector2f pos = {
            worldBounds.left + static_cast<float>(rand()) / RAND_MAX * worldBounds.width,
            worldBounds.top + static_cast<float>(rand()) / RAND_MAX * worldBounds.height
        };
        worldObjects.emplace_back(WorldObject::Type::Stone, pos);
    }

    // ======================================================
    // ZENE
    // ======================================================
    sf::Music music;
    if (!music.openFromFile("resources/music.wav"))
        return -1;

    music.setLoop(true);
    music.setVolume(40.f);
    music.play();

    // ======================================================
    // HANGEFFEKTEK
    // ======================================================
    sf::SoundBuffer throwBuf, hitBuf;
    if (!throwBuf.loadFromFile("resources/throw.wav"))
        return -1;
    if (!hitBuf.loadFromFile("resources/hit.wav"))
        return -1;

    sf::Sound throwSound(throwBuf);
    throwSound.setVolume(60.f);

    sf::Sound hitSound(hitBuf);
    hitSound.setVolume(90.f);

    // ======================================================
    // FONT + HUD
    // ======================================================
    sf::Font font;
    if (!font.loadFromFile("resources/font.ttf"))
        return -1;

    sf::Text hudText;
    hudText.setFont(font);
    hudText.setCharacterSize(20);
    hudText.setFillColor(sf::Color::White);

    sf::Text waveText;
    waveText.setFont(font);
    waveText.setCharacterSize(48);
    waveText.setFillColor(sf::Color::White);
    waveText.setOutlineColor(sf::Color::Black);
    waveText.setOutlineThickness(3.f);

    // ================= HOME SCREEN UI =================
    sf::Text homeTitle;
    homeTitle.setFont(font);
    homeTitle.setString("SnowFighter");
    homeTitle.setCharacterSize(96);
    homeTitle.setFillColor(sf::Color::White);
    homeTitle.setOutlineColor(sf::Color::Black);
    homeTitle.setOutlineThickness(5.f);

    sf::RectangleShape playButton({ 320.f, 80.f });
    playButton.setFillColor(sf::Color(180, 40, 40));

    sf::Text playText("PLAY", font, 36);
    playText.setFillColor(sf::Color::White);

    // ================= GAME OVER UI =================
    sf::Text gameOverTitle;
    gameOverTitle.setFont(font);
    gameOverTitle.setString("CHRISTMAS OVER");
    gameOverTitle.setCharacterSize(72);
    gameOverTitle.setFillColor(sf::Color::White);
    gameOverTitle.setOutlineColor(sf::Color::Black);
    gameOverTitle.setOutlineThickness(4.f);

    sf::RectangleShape replayButton({ 260.f, 60.f });
    sf::RectangleShape homeButton({ 260.f, 60.f });

    replayButton.setFillColor(sf::Color(180, 40, 40));
    homeButton.setFillColor(sf::Color(40, 40, 40));

    sf::Text replayText("REPLAY", font, 28);
    sf::Text homeText("HOME", font, 28);

    replayText.setFillColor(sf::Color::White);
    homeText.setFillColor(sf::Color::White);

    // ======================================================
    // CLOCKS
    // ======================================================
    sf::Clock deltaClock;
    sf::Clock fpsClock;
    sf::Clock waveSpawnClock;
    sf::Clock waveRestClock;

    int fpsFrames = 0;
    float fps = 0.f;

    // ======================================================
    // WAVE SYSTEM
    // ======================================================
    int currentWave = 1;
    bool waveActive = false;
    bool waveResting = true;

    int enemiesToSpawn = GetEnemiesToSpawnForWave(currentWave);
    int enemiesSpawned = 0;

    // ======================================================
    // PLAYER DAMAGE COOLDOWN (ne 60 fps-sel sebezzen)
    // ======================================================
    sf::Clock playerDamageClock;
    const float PLAYER_HIT_COOLDOWN = 0.6f;

    // ======================================================
    // MAIN LOOP
    // ======================================================
    while (window.isOpen())
    {
        float dt = deltaClock.restart().asSeconds();

        // ================= EVENTS =================
        sf::Event ev;
        while (window.pollEvent(ev))
        {
            if (ev.type == sf::Event::Closed ||
                (ev.type == sf::Event::KeyPressed &&
                    ev.key.code == sf::Keyboard::Escape))
                window.close();

            if (ev.type == sf::Event::MouseButtonPressed &&
                ev.mouseButton.button == sf::Mouse::Left)
            {
                player.startThrow();

                sf::Vector2f mouseWorld =
                    window.mapPixelToCoords(
                        { ev.mouseButton.x, ev.mouseButton.y },
                        camera
                    );

                snowballs.emplace_back(
                    player.getPosition(),
                    mouseWorld - player.getPosition()
                );

                throwSound.play();
            }
        }
        if (gameState == GameState::Home &&
            ev.type == sf::Event::MouseButtonPressed &&
            ev.mouseButton.button == sf::Mouse::Left)
        {
            sf::Vector2f mouse =
                window.mapPixelToCoords(
                    { ev.mouseButton.x, ev.mouseButton.y },
                    hudView
                );

            if (playButton.getGlobalBounds().contains(mouse))
            {
                // ===== JÁTÉK INDÍTÁS =====
                player = Character();

                enemies.clear();
                rangedEnemies.clear();
                snowballs.clear();
                fireballs.clear();
                healthPickups.clear();

                waveActive = false;
                waveResting = true;
                currentWave = 1;
                enemiesSpawned = 0;
                enemiesToSpawn = GetEnemiesToSpawnForWave(currentWave);
                waveRestClock.restart();

                gameState = GameState::Playing;
            }
        }

        if (gameState == GameState::GameOver &&
            ev.type == sf::Event::MouseButtonPressed &&
            ev.mouseButton.button == sf::Mouse::Left)
        {
            sf::Vector2f mouse =
                window.mapPixelToCoords(
                    { ev.mouseButton.x, ev.mouseButton.y },
                    hudView
                );

            if (replayButton.getGlobalBounds().contains(mouse))
            {
                // ===== RESET JÁTÉK =====
                player = Character();

                enemies.clear();
                rangedEnemies.clear();
                snowballs.clear();
                fireballs.clear();
                healthPickups.clear();

                currentWave = 1;
                waveActive = false;
                waveResting = true;
                enemiesSpawned = 0;
                enemiesToSpawn = GetEnemiesToSpawnForWave(currentWave);
                waveRestClock.restart();

                gameState = GameState::Playing;
            }
        }
        if (gameState == GameState::GameOver &&
            ev.type == sf::Event::MouseButtonPressed &&
            ev.mouseButton.button == sf::Mouse::Left)
        {
            sf::Vector2f mouse(
                static_cast<float>(ev.mouseButton.x),
                static_cast<float>(ev.mouseButton.y)
            );

            if (homeButton.getGlobalBounds().contains(mouse))
            {
                // 👉 HOME LOGIKA
                gameState = GameState::Home;

                // TELJES RESET
                enemies.clear();
                rangedEnemies.clear();
                snowballs.clear();
                fireballs.clear();
                healthPickups.clear();

                waveActive = false;
                waveResting = true;
                currentWave = 1;
                enemiesSpawned = 0;
                enemiesToSpawn = GetEnemiesToSpawnForWave(currentWave);

                player.reset();   // FONTOS (HP, pozíció, anim)
            }
        }



        // ================= UPDATE PLAYER =================
        sf::Vector2f oldPos = player.getPosition();
        player.handleInput(dt);
        player.update(dt);

        // kő collision (ne menj át)
        for (auto& obj : worldObjects)
        {
            if (obj.getType() == WorldObject::Type::Stone &&
                obj.getCollisionBounds().intersects(player.getBounds()))
            {
                player.setPosition(oldPos);
                break;
            }
        }
        sf::Vector2f pos = player.getPosition();

        // sprite méret fele (nálad ~96x96)
        const float halfW = 48.f;
        const float halfH = 48.f;

        pos.x = std::clamp(
            pos.x,
            worldBounds.left + halfW,
            worldBounds.left + worldBounds.width - halfW
        );

        pos.y = std::clamp(
            pos.y,
            worldBounds.top + halfH,
            worldBounds.top + worldBounds.height - halfH
        );

        player.setPosition(pos);


        camera.setCenter(player.getPosition());

        // ================= WAVE LOGIKA =================
        if (gameState == GameState::Playing)
        {
            // ================= WAVE LOGIKA =================
            if (waveResting)
            {
                if (waveRestClock.getElapsedTime().asSeconds() >= GetRestTimeBetweenWaves())
                {
                    waveResting = false;
                    waveActive = true;
                    enemiesSpawned = 0;
                    enemiesToSpawn = GetEnemiesToSpawnForWave(currentWave);
                    waveSpawnClock.restart();
                }
            }

            if (waveActive)
            {
                if (enemiesSpawned < enemiesToSpawn &&
                    waveSpawnClock.getElapsedTime().asSeconds() >=
                    GetSpawnDelayForWave(currentWave))
                {
                    waveSpawnClock.restart();

                    sf::Vector2f spawn;
                    do
                    {
                        spawn = {
                            worldBounds.left + static_cast<float>(rand()) / RAND_MAX * worldBounds.width,
                            worldBounds.top + static_cast<float>(rand()) / RAND_MAX * worldBounds.height
                        };
                    } while (std::hypot(
                        spawn.x - player.getPosition().x,
                        spawn.y - player.getPosition().y) < 220.f);

                    if (rand() % 4 == 0)
                        rangedEnemies.emplace_back(spawn);
                    else
                        enemies.emplace_back(spawn);

                    enemiesSpawned++;
                }

                if (enemiesSpawned >= enemiesToSpawn &&
                    enemies.empty() &&
                    rangedEnemies.empty())
                {
                    waveActive = false;
                    waveResting = true;
                    currentWave++;
                    waveRestClock.restart();
                }
            }
        }


        if (waveActive)
        {
            if (enemiesSpawned < enemiesToSpawn &&
                waveSpawnClock.getElapsedTime().asSeconds() >=
                GetSpawnDelayForWave(currentWave))
            {
                waveSpawnClock.restart();

                sf::Vector2f spawn;
                do
                {
                    spawn = {
                        worldBounds.left + static_cast<float>(rand()) / RAND_MAX * worldBounds.width,
                        worldBounds.top + static_cast<float>(rand()) / RAND_MAX * worldBounds.height
                    };
                } while (std::hypot(
                    spawn.x - player.getPosition().x,
                    spawn.y - player.getPosition().y) < 220.f);

                // 25% ranged
                if (rand() % 4 == 0)
                    rangedEnemies.emplace_back(spawn);
                else
                    enemies.emplace_back(spawn);

                enemiesSpawned++;
            }

            if (enemiesSpawned >= enemiesToSpawn &&
                enemies.empty() &&
                rangedEnemies.empty())
            {
                waveActive = false;
                waveResting = true;
                currentWave++;
                waveRestClock.restart();
            }
        }

        // ================= UPDATE ENTITIES =================
        for (auto& s : snowballs) s.update(dt);
        for (auto& f : fireballs) f.update(dt);
        for (auto& e : enemies) e.update(dt, player.getPosition(), enemies);
        for (auto& r : rangedEnemies) r.update(dt, player.getPosition(), fireballs);
        if (gameState == GameState::Home)
        {
            for (auto& s : snowflakes)
            {
                s.shape.move(0.f, s.speed * dt);

                // ha leért, felül újra
                if (s.shape.getPosition().y > desktop.height)
                {
                    s.shape.setPosition(
                        static_cast<float>(rand() % desktop.width),
                        -10.f
                    );
                }
            }
        }


        // ================= COLLISIONS =================

        // snowball -> melee enemy
        for (auto& e : enemies)
        {
            for (auto& s : snowballs)
            {
                if (e.isAlive() && s.isAlive() &&
                    e.getBounds().intersects(s.getBounds()))
                {
                    e.takeDamage(1, s.getVelocity());
                    s.kill();
                    if (hitSound.getStatus() != sf::Sound::Playing)
                        hitSound.play();
                }
            }
        }

        // snowball -> ranged enemy
        for (auto& r : rangedEnemies)
        {
            for (auto& s : snowballs)
            {
                if (r.isAlive() && s.isAlive() &&
                    r.getBounds().intersects(s.getBounds()))
                {
                    r.takeDamage(1);
                    s.kill();
                    if (hitSound.getStatus() != sf::Sound::Playing)
                        hitSound.play();
                }
            }
        }

        // fireball -> player (SEBZÉS)
        for (auto& f : fireballs)
        {
            if (f.isAlive() && f.getBounds().intersects(player.getBounds()))
            {
                player.takeDamage(1);
                f.kill();
            }
        }

        // melee enemy -> player (SEBZÉS + cooldown)
        if (playerDamageClock.getElapsedTime().asSeconds() >= PLAYER_HIT_COOLDOWN)
        {
            bool gotHit = false;

            for (auto& e : enemies)
            {
                if (e.isAlive() && e.getBounds().intersects(player.getBounds()))
                {
                    gotHit = true;
                    break;
                }
            }

            for (auto& r : rangedEnemies)
            {
                if (r.isAlive() && r.getBounds().intersects(player.getBounds()))
                {
                    gotHit = true;
                    break;
                }
            }

            if (gotHit)
            {
                player.takeDamage(1.5);
                playerDamageClock.restart();
            }
        }

        // player -> heal pickup
        for (auto& h : healthPickups)
        {
            if (h.isAlive() &&
                h.getBounds().intersects(player.getBounds()))
            {
                player.heal(h.getHealAmount());
                h.kill();
            }
        }

        // ================= CLEANUP =================
        snowballs.erase(std::remove_if(
            snowballs.begin(), snowballs.end(),
            [](const Snowball& s) { return !s.isAlive(); }),
            snowballs.end());

        fireballs.erase(std::remove_if(
            fireballs.begin(), fireballs.end(),
            [](const Fireball& f) { return !f.isAlive(); }),
            fireballs.end());
        for (auto& e : enemies)
        {
            if (!e.isAlive())
            {
                if (rand() % 2 == 0) // 50%
                {
                    healthPickups.emplace_back(e.getPosition());
                }
            }
        }

        enemies.erase(std::remove_if(
            enemies.begin(), enemies.end(),
            [](const Enemy& e) { return !e.isAlive(); }),
            enemies.end());
        for (auto& e : rangedEnemies)
        {
            if (!e.isAlive())
            {
                if (rand() % 2 == 0)
                {
                    healthPickups.emplace_back(e.getPosition());
                }
            }
        }

        rangedEnemies.erase(
            std::remove_if(
                rangedEnemies.begin(),
                rangedEnemies.end(),
                [](const RangedEnemy& e) { return !e.isAlive(); }
            ),
            rangedEnemies.end()
        );

        rangedEnemies.erase(std::remove_if(
            rangedEnemies.begin(), rangedEnemies.end(),
            [](const RangedEnemy& r) { return !r.isAlive(); }),
            rangedEnemies.end());

        healthPickups.erase(std::remove_if(
            healthPickups.begin(), healthPickups.end(),
            [](const HealthPickup& h) { return !h.isAlive(); }),
            healthPickups.end());

        // ================= GAME OVER =================
        if (!player.isAlive())
            gameState = GameState::GameOver;

        // ================= HUD TEXT =================
        int enemiesAlive = static_cast<int>(enemies.size() + rangedEnemies.size());
        int enemiesLeft = (enemiesToSpawn - enemiesSpawned) + enemiesAlive;

        float nextWaveIn = 0.f;
        if (waveResting)
        {
            nextWaveIn =
                GetRestTimeBetweenWaves() -
                waveRestClock.getElapsedTime().asSeconds();
            if (nextWaveIn < 0.f) nextWaveIn = 0.f;
        }

        if (waveResting)
        {
            waveText.setString(
                "WAVE " + std::to_string(currentWave - 1) +
                " COMPLETED\nEnemies left: 0\nNext wave in: " +
                std::to_string((int)std::ceil(nextWaveIn)) + "s"
            );
        }
        else
        {
            waveText.setString(
                "WAVE " + std::to_string(currentWave) +
                "\nEnemies left: " + std::to_string(enemiesLeft)
            );
        }

        sf::FloatRect wb = waveText.getLocalBounds();
        waveText.setOrigin(wb.left + wb.width / 2.f, 0.f);
        waveText.setPosition(static_cast<float>(desktop.width) / 2.f, 20.f);

        // FPS
        fpsFrames++;
        if (fpsClock.getElapsedTime().asSeconds() >= 1.f)
        {
            fps = fpsFrames / fpsClock.getElapsedTime().asSeconds();
            fpsFrames = 0;
            fpsClock.restart();
        }
        hudText.setString(
            "SnowFighter " + VERSION +
            "\nFPS: " + std::to_string(static_cast<int>(fps))
        );

        // FONT BOUNDS
        sf::FloatRect bounds = hudText.getLocalBounds();

        // ORIGIN: jobb felső sarok
        hudText.setOrigin(
            bounds.left + bounds.width,
            bounds.top
        );

        // POZÍCIÓ: képernyő jobb felső sarka
        hudText.setPosition(
            static_cast<float>(desktop.width) - 10.f,
            10.f
        );
        ;
        sf::Vector2f camCenter = player.getPosition();
        sf::Vector2f camHalf = camera.getSize() * 0.5f;

        // X tengely
        camCenter.x = std::clamp(
            camCenter.x,
            worldBounds.left + camHalf.x,
            worldBounds.left + worldBounds.width - camHalf.x
        );

        // Y tengely
        camCenter.y = std::clamp(
            camCenter.y,
            worldBounds.top + camHalf.y,
            worldBounds.top + worldBounds.height - camHalf.y
        );

        camera.setCenter(camCenter);


        // ================= RENDER =================
        window.clear();
        window.setView(camera);

        window.draw(floor);

        // stones under
        for (auto& o : worldObjects)
            if (o.getType() == WorldObject::Type::Stone)
                o.draw(window);

        player.draw(window);
		if (gameState == GameState::Playing)
        {
            for (auto& e : enemies) e.draw(window);
            for (auto& r : rangedEnemies) r.draw(window);
            for (auto& s : snowballs) s.draw(window);
            for (auto& f : fireballs) f.draw(window);
        }
        for (auto& h : healthPickups) h.draw(window);

        // trees over (takarnak)
        for (auto& o : worldObjects)
            if (o.getType() == WorldObject::Type::Tree)
                o.draw(window);

        // --- HUD ---
        window.setView(hudView);

        if (gameState == GameState::GameOver)
        {
            window.setView(hudView);

            // sötét overlay
            sf::RectangleShape overlay({
                static_cast<float>(desktop.width),
                static_cast<float>(desktop.height)
                });
            overlay.setFillColor(sf::Color(0, 0, 0, 180));
            window.draw(overlay);

            // cím
            sf::FloatRect t = gameOverTitle.getLocalBounds();
            gameOverTitle.setOrigin(t.width / 2.f, t.height / 2.f);
            gameOverTitle.setPosition(
                desktop.width / 2.f,
                desktop.height / 2.f - 140.f
            );

            // gombok
            replayButton.setPosition(
                desktop.width / 2.f - 130.f,
                desktop.height / 2.f - 20.f
            );

            homeButton.setPosition(
                desktop.width / 2.f - 130.f,
                desktop.height / 2.f + 60.f
            );

            replayText.setPosition(
                replayButton.getPosition().x + 80.f,
                replayButton.getPosition().y + 14.f
            );

            homeText.setPosition(
                homeButton.getPosition().x + 90.f,
                homeButton.getPosition().y + 14.f
            );

            window.draw(gameOverTitle);
            window.draw(replayButton);
            window.draw(homeButton);
            window.draw(replayText);
            window.draw(homeText);
        }
        if (gameState == GameState::Home)
        {
            window.setView(hudView);

            // háttér sötétítés
            sf::RectangleShape overlay({
                static_cast<float>(desktop.width),
                static_cast<float>(desktop.height)
                });
            overlay.setFillColor(sf::Color(0, 0, 0, 200));
            window.draw(overlay);

            // cím középen
            sf::FloatRect tb = homeTitle.getLocalBounds();
            homeTitle.setOrigin(tb.width / 2.f, tb.height / 2.f);
            homeTitle.setPosition(
                desktop.width / 2.f,
                desktop.height / 2.f - 140.f
            );

            // Play gomb
            playButton.setPosition(
                desktop.width / 2.f - playButton.getSize().x / 2.f,
                desktop.height / 2.f
            );

            playText.setPosition(
                playButton.getPosition().x + 110.f,
                playButton.getPosition().y + 20.f
            );

            window.draw(homeTitle);
            window.draw(playButton);
            window.draw(playText);
        }
        if (gameState == GameState::Home)
        {
            window.setView(hudView);

            // ❄️ hóesés
            for (auto& s : snowflakes)
                window.draw(s.shape);

            // 🎄 főmenü elemek
            window.draw(homeTitle);   // SnowFighter
            window.draw(playButton);
            window.draw(playText);
        }



        // HP bar
        float hpRatio =
            static_cast<float>(player.getHealth()) /
            static_cast<float>(player.getMaxHealth());
        hpRatio = std::clamp(hpRatio, 0.f, 1.f);

        sf::RectangleShape hpBg({ 300.f, 20.f });
        hpBg.setFillColor(sf::Color(40, 40, 40));
        hpBg.setPosition(20.f, 20.f);

        sf::RectangleShape hpBar({ 300.f * hpRatio, 20.f });
        hpBar.setFillColor(sf::Color(100, 220, 100));
        hpBar.setPosition(20.f, 20.f);

        window.draw(hpBg);
        if (gameState == GameState::Playing || gameState == GameState::GameOver) {
            window.draw(hpBar);
		}

        window.draw(hudText);
        if (gameState == GameState::Playing || gameState == GameState::GameOver)
        {
            window.draw(waveText);
        }

        window.display();
    }

    return 0;
}
