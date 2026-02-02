#include "Character.h"
#include <cmath>
#include <algorithm>

// ======================================================
// SEGÉDFÜGGVÉNY
// ======================================================
static sf::Vector2f normalize(sf::Vector2f v)
{
    float l = std::sqrt(v.x * v.x + v.y * v.y);
    if (l == 0.f) return { 0.f, 0.f };
    return v / l;
}

// ======================================================
// STATIKUS TEXTÚRÁK
// ======================================================
sf::Texture Character::s_idle;
sf::Texture Character::s_throw;
sf::Texture Character::s_up1;
sf::Texture Character::s_up2;
sf::Texture Character::s_right1;
sf::Texture Character::s_right2;
sf::Texture Character::s_left1;
sf::Texture Character::s_left2;

// ======================================================
// KONSTRUKTOR
// ======================================================
Character::Character()
{
    static bool loaded = false;
    if (!loaded)
    {
        s_idle.loadFromFile("resources/santa1.png");
        s_throw.loadFromFile("resources/santa2.png");

        s_up1.loadFromFile("resources/santa3.png");
        s_up2.loadFromFile("resources/santa4.png");

        s_right1.loadFromFile("resources/santa5.png");
        s_right2.loadFromFile("resources/santa6.png");

        s_left1.loadFromFile("resources/santa7.png");
        s_left2.loadFromFile("resources/santa8.png");

        loaded = true;
    }

    m_sprite.setTexture(s_idle);
    m_sprite.setOrigin(16.f, 16.f);
    m_sprite.setScale(3.f, 3.f);
    m_sprite.setPosition(0.f, 0.f);
}

// ======================================================
// INPUT
// ======================================================
void Character::handleInput(float dt)
{
    m_velocity = { 0.f, 0.f };

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        m_velocity.y -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        m_velocity.y += 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        m_velocity.x -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        m_velocity.x += 1.f;

    if (m_velocity.x != 0.f || m_velocity.y != 0.f)
        m_velocity = normalize(m_velocity) * m_speed;
}

// ======================================================
// UPDATE
// ======================================================
void Character::update(float dt)
{
    if (!m_alive) return;

    m_sprite.move(m_velocity * dt);
    updateAnimation();
}

// ======================================================
// ANIMÁCIÓ
// ======================================================
void Character::updateAnimation()
{
    // ===== DOBÁS ANIM =====
    if (m_throwing)
    {
        if (m_throwClock.getElapsedTime().asSeconds() < 0.25f)
        {
            m_sprite.setTexture(s_throw);
            return;
        }
        m_throwing = false;
    }

    if (m_animClock.getElapsedTime().asSeconds() < 0.2f)
        return;

    m_animClock.restart();
    m_animFrame = 1 - m_animFrame;

    // ===== MOZGÁS ALAPÚ ANIM =====
    if (std::abs(m_velocity.y) > std::abs(m_velocity.x))
    {
        if (m_velocity.y != 0.f)
        {
            m_sprite.setTexture(
                m_animFrame == 0 ? s_up1 : s_up2
            );
            return;
        }
    }

    if (m_velocity.x > 0.f)
    {
        m_sprite.setTexture(
            m_animFrame == 0 ? s_right1 : s_right2
        );
    }
    else if (m_velocity.x < 0.f)
    {
        m_sprite.setTexture(
            m_animFrame == 0 ? s_left1 : s_left2
        );
    }
    else
    {
        m_sprite.setTexture(s_idle);
    }
}

// ======================================================
// DRAW
// ======================================================
void Character::draw(sf::RenderWindow& window)
{
    if (m_alive)
        window.draw(m_sprite);
}

// ======================================================
// DOBÁS
// ======================================================
void Character::startThrow()
{
    m_throwing = true;
    m_throwClock.restart();
}

// ======================================================
// HP
// ======================================================
void Character::takeDamage(int dmg)
{
    m_health -= dmg;
    if (m_health <= 0)
        m_alive = false;
}

void Character::heal(int amount)
{
    m_health = std::min(m_health + amount, m_maxHealth);
}

int Character::getHealth() const { return m_health; }
int Character::getMaxHealth() const { return m_maxHealth; }
bool Character::isAlive() const { return m_alive; }

void Character::reset()
{
    m_health = m_maxHealth;
    m_sprite.setPosition(0.f, 0.f);
    m_velocity = { 0.f, 0.f };
    m_alive = true;

}

// ======================================================
// COLLISION / POSITION
// ======================================================
sf::FloatRect Character::getBounds() const
{
    return m_sprite.getGlobalBounds();
}

sf::Vector2f Character::getPosition() const
{
    return m_sprite.getPosition();
}

void Character::setPosition(sf::Vector2f pos)
{
    m_sprite.setPosition(pos);
}
