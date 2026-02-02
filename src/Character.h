#pragma once
#include <SFML/Graphics.hpp>

class Character
{
public:
    Character();

    void handleInput(float dt);
    void update(float dt);
    void draw(sf::RenderWindow& window);

    void startThrow();

    // ===== HP =====
    void takeDamage(int dmg);
    void heal(int amount);

    int getHealth() const;
    int getMaxHealth() const;
    bool isAlive() const;

    void reset();


    // ===== COLLISION / POSITION =====
    sf::FloatRect getBounds() const;
    sf::Vector2f getPosition() const;
    void setPosition(sf::Vector2f pos);

private:
    void updateAnimation();

private:
    sf::Sprite m_sprite;

    // ===== STATIKUS TEXTÚRÁK =====
    static sf::Texture s_idle;      // santa1
    static sf::Texture s_throw;     // santa2
    static sf::Texture s_up1;       // santa3
    static sf::Texture s_up2;       // santa4
    static sf::Texture s_right1;    // santa5
    static sf::Texture s_right2;    // santa6
    static sf::Texture s_left1;     // santa7
    static sf::Texture s_left2;     // santa8

    // ===== MOZGÁS =====
    sf::Vector2f m_velocity;
    float m_speed = 220.f;

    // ===== ANIM =====
    int m_animFrame = 0;
    sf::Clock m_animClock;

    // ===== DOBÁS =====
    bool m_throwing = false;
    sf::Clock m_throwClock;

    // ===== HP =====
    int m_health = 10;
    int m_maxHealth = 10;
    bool m_alive = true;
};
