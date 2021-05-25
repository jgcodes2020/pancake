#include "pancake/sm64.hpp"
#include "pancake/properties.hpp"

#ifndef __pancake_GOBJECTS_HPP
#define __pancake_GOBJECTS_HPP

#include <cstdint>
#include <stdexcept>

namespace pancake {
  /**
   * @brief Represents physical Mario (aka the Mario struct).
   */
  class mario {
  private:
    sm64& m_game;
    float* m_x_ptr, * m_y_ptr, * m_z_ptr;
    float* m_xvel_ptr, * m_yvel_ptr, * m_zvel_ptr;
    float* m_hspeed_ptr;


  public:
    mario(sm64& game) :
      m_game(game),
      m_x_ptr(this->m_game.locate<float>("gMarioStates", 0x3C)),
      m_y_ptr(this->m_game.locate<float>("gMarioStates", 0x40)),
      m_z_ptr(this->m_game.locate<float>("gMarioStates", 0x44)),
      
      m_xvel_ptr(this->m_game.locate<float>("gMarioStates", 0x48)),
      m_yvel_ptr(this->m_game.locate<float>("gMarioStates", 0x4C)),
      m_zvel_ptr(this->m_game.locate<float>("gMarioStates", 0x50)),
      
      m_hspeed_ptr(this->m_game.locate<float>("gMarioStates", 0x50)) {
    };

    /**
     * @brief Reference to Mario's X position.
     */
    float& x = *(m_x_ptr);

    /**
     * @brief Reference to Mario's Y position.
     */
    float& y = *(m_y_ptr);

    /**
     * @brief Reference to Mario's Z position.
     */
    float& z = *(m_z_ptr);

    /**
     * @brief Reference to physical Mario's X velocity.
     */
    float& xvel = *(m_xvel_ptr);

    /**
     * @brief Reference to physical Mario's Y velocity.
     */
    float& yvel = *(m_yvel_ptr);
    /**
     * @brief Reference to physical Mario's Y velocity.
     */
    float& vspeed = *(m_yvel_ptr);

    /**
     * @brief Reference to physical Mario's Z velocity.
     */
    float& zvel = *(m_zvel_ptr);

    /**
     * @brief Reference to physical Mario's H speed.
     */
    float& hspeed = *(m_hspeed_ptr);
  };

  /**
   * @brief Represents an object (one in an object slot).
   */
  class g_object {
  public:
    
  protected:
    sm64& m_game;
    uint8_t m_slot;

    float* m_x_ptr, * m_y_ptr, * m_z_ptr, * m_hspeed_ptr;
    uint16_t* m_yaw_vel_ptr;
    bool* m_state_held_ptr;
  public:
    g_object() = delete;
    g_object(const g_object&) = delete;
    g_object(g_object&&) = delete;

    g_object(sm64& game, uint8_t slot) :
      m_game(game), m_slot(slot),
      m_x_ptr(m_game.locate<float>("gObjectPool", (slot * 0x570L) + 0x0F0)),
      m_y_ptr(m_game.locate<float>("gObjectPool", (slot * 0x570L) + 0x0F4)),
      m_z_ptr(m_game.locate<float>("gObjectPool", (slot * 0x570L) + 0x0F8)),

      m_hspeed_ptr(m_game.locate<float>("gObjectPool", (slot * 0x570L) + 0x108)),
      m_yaw_vel_ptr(m_game.locate<uint16_t>("gObjectPool", (slot * 0x570L) + 0x118)),

      base_ptr(m_game.locate<void>("gObjectPool", (slot * 0x570L))) {

    }

    /**
     * @brief Base pointer of the object. Use this to obtain properties not already exposed by the API.
     */
    const void* base_ptr;

    /**
     * @brief Reference to the object's X position.
     */
    float& x = *(m_x_ptr);

    /**
     * @brief Reference to the object's Y position.
     */
    float& y = *(m_y_ptr);

    /**
     * @brief Reference to the object's Z position.
     */
    float& z = *(m_z_ptr);

    /**
     * @brief Reference to the object's H speed.
     */
    float& hspeed = *(m_hspeed_ptr);

    /**
     * @brief Reference to the object's yaw velocity.
     */
    uint16_t& yaw_vel = *(m_yaw_vel_ptr);

    /**
     * Copy behaviour of this object to another object.
     * @param that another object.
     */
    void copy_bhv(g_object that) {
      uint8_t* const src_ptr = m_game.locate<uint8_t>("gObjectPool", (this->m_slot * 1392) + 0x0A0);
      uint8_t* const dst_ptr = m_game.locate<uint8_t>("gObjectPool", (that.m_slot * 1392) + 0x0A0);
      memmove(dst_ptr, src_ptr, 1232);
    }
  };

  /**
   * @brief Represents a bully. (Yeah, those black viking orb thingies.)
   * 
   */
  class bully : public g_object {
  public:
    /**
     * @brief A subtype representing a bully.
     */
    enum class type : uint16_t {
      /**
       * @brief Represents a normal bully.
       */
      GENERIC = 0,
      /**
       * @brief Represents a minion bully. Used in Bully the Bullies (LLL) to manage big bully spawning.
       */
      MINION = 1,
      /**
       * @brief Represents a chill bully.
       */
      CHILL = 16
    };
  private:
    uint16_t* m_subtype_ptr;
    uint32_t* m_kb_timer_ptr;

  public:
    bully(sm64& game, uint8_t slot) :
      g_object(game, slot),
      m_subtype_ptr(m_game.locate<uint16_t>("gObjectPool", (slot * 0x570L) + 0x0F4)),
      m_kb_timer_ptr(m_game.locate<uint32_t>("gObjectPool", (slot * 0x570L) + 0x104)),

      subtype(
      [this]() {
        return static_cast<type>(*(this->m_subtype_ptr));
      },
      [this](type t) {
        *m_subtype_ptr = static_cast<uint16_t>(t);
      }) {    }

    /**
     * @brief Subtype of this bully.
     */
    property<type> subtype;

    /**
     * @brief The amount of time this bully has been in KB.
     *
     */
    uint32_t& kb_timer = *(m_kb_timer_ptr);
    /**
     * @brief Indicates the number of small bullies killed.
     */
    uint32_t& minions_killed = *(m_kb_timer_ptr);
  };
}
#endif