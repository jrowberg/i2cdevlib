/*
 * Arduino library for the MMA8452Q three-axis accelerometer.
 *
 * Copyright (c) 2013, Alessandro Ghedini
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef MMA8452Q_H
#define MMA8452Q_H

#include <stdint.h>

class MMA8452Q {
public:
/* enum MMA8452Q_INTERRUPTS { */
/* 	MMA8452Q_INT1 = 1, */
/* 	MMA8452Q_INT2 = 0 */
/* }; */

enum MMA8452Q_SYSMOD {
	STANDBY, /**< Stand-by system mode */
	WAKE,    /**< Wake system mode */
	SLEEP    /**< Sleep system mode */
};

enum MMA8452Q_EVENTS {
	FREEFALL_MOTION = 0x01, /**< Freefall/motion event */
	PULSE           = 0x02, /**< Pulse (tap) event */
	ORIENTATION     = 0x04  /**< Orientation change event */
};

MMA8452Q();

/*! Initialize the MMA8452Q. */
int begin(void);

/*! Read the status of the MMA8452Q. */
uint8_t status(void);

/*! Read the system mode of the MMA8452Q.

\return One of the MMA8452Q::MMA8452Q_SYSMOD values.

Example:
\verbatim embed:rst
.. code-block:: c++

  switch (accel.sysmod()) {
    case MMA8452Q::STANDBY:
      Serial.println("StandBy");
      break;

    case MMA8452Q::SLEEP:
      Serial.println("Sleep");
      break;

    case MMA8452Q::WAKE:
      Serial.println("Wake");
      break;
  }
\endverbatim
 */
uint8_t sysmod(void);

/*! Read the source pin of the last interrupt. */
uint8_t intSource(void);


/*! Set the scale of the MMA8452Q.

Before calling this method, the MMA8452Q must be disabled using the active()
method.

\param scale scale can be 2, 4, or 8.

Example:
\verbatim embed:rst
.. code-block:: c++

  accel.active(false);
  accel.scale(2);
  accel.active(true);
\endverbatim
 */
void scale(uint8_t scale);

/*! Calibrate the MMA8452Q.

Before calling this method, the MMA8452Q must be disabled using the active()
method.

\param off_x X axis calibration.
\param off_y Y axis calibration.
\param off_z Z axis calibration.

Example:
\verbatim embed:rst
.. code-block:: c++

  accel.active(false);
  accel.offset(5, -6, 10);
  accel.active(true);
\endverbatim
 */

void offset(int8_t off_x, int8_t off_y, int8_t off_z);

/*! Read the raw values of the axes.

\param axes output array of size 3 (must be allocated by the user).

Example:
\verbatim embed:rst
.. code-block:: c++

  int axes[3];
  int x, y, z;

  accel.axes(axes);

  x = axes[0];
  y = axes[1];
  x = axes[2];
\endverbatim
 */


void axes(int *axes);

/*! Read the orientation value.

Before calling this method, the orientation detection must be activated using
the detectOrientation() method.

\param value output value.
\return Whether the orientation has changed since the last read.

Example:
\verbatim embed:rst
.. code-block:: c++

  uint8_t orientation;

  if (accel.orientation(&orientation)) {
    ...
  }
\endverbatim
 */
bool orientation(uint8_t *value);

/*! Read the landscape orientation status.

\param orientation The orientation set by the orientation() method.
\return Whether the landscape orientation is right or left.

Example:
\verbatim embed:rst
.. code-block:: c++

  uint8_t orientation;

  accel.orientation(&orientation));

  switch (accel.landscape(orientation)) {
    case HIGH:
      Serial.println("Landscape Right");
      break;

    case LOW:
      Serial.println("Landscape Left");
      break;

    default:
      Serial.println("No Orientation");
      break;
  }
\endverbatim
 */
int landscape(uint8_t orientation);

/*! Read the portrait orientation status.

\param orientation The orientation set by the orientation() method.
\return Whether the portrait orientation is up or down.

Example:
\verbatim embed:rst
.. code-block:: c++

  uint8_t orientation;

  accel.orientation(&orientation));

  switch (accel.portrait(orientation)) {
    case HIGH:
      Serial.println("Portrait Up");
      break;

    case LOW:
      Serial.println("Portrait Down");
      break;

    default:
      Serial.println("No Portrait");
      break;
  }
\endverbatim
 */
int portrait(uint8_t orientation);

/*! Read the back/front orientation status.

\param orientation The orientation set by the orientation() method.
\return Whether the orientation is back or front.

Example:
\verbatim embed:rst
.. code-block:: c++

  uint8_t orientation;

  accel.orientation(&orientation));

  if (accel.backFront(orientation))
    Serial.println("Back");
  else
    Serial.println("Front");
\endverbatim
 */
int backFront(uint8_t orientation);


/*! Enable/disable the MMA8452Q.

\param enable Whether to enable or disable the MMA8452Q.
 */
void active(bool enable);

/*! Enable/disable the fast read mode.

Before calling this method, the MMA8452Q must be disabled using the active()
method.

\param enable Whether to enable or disable the fast read mode.

Example:
\verbatim embed:rst
.. code-block:: c++

  accel.active(false);
  accel.fastRead(true);
  accel.active(true);
\endverbatim
 */
void fastRead(bool enable);

/*! Enable/disable the low noise mode.

Before calling this method, the MMA8452Q must be disabled using the active()
method.

\param enable Whether to enable or disable the low noise mode.

Example:
\verbatim embed:rst
.. code-block:: c++

  accel.active(false);
  accel.lowNoise(true);
  accel.active(true);
\endverbatim
 */
void lowNoise(bool enable);


/*! Reset the MMA8452Q. */
void reset(void);

/*! Enable/disable the self-test mode.

Before calling this method, the MMA8452Q must be disabled using the active()
method.

\param enable Whether to enable or disable the self-test mode.

Example:
\verbatim embed:rst
.. code-block:: c++

  accel.active(false);
  accel.selfTest(true);
  accel.active(true);
\endverbatim
 */
void selfTest(bool enable);

/*! Enable/disable the auto-sleep mode.

Before calling this method, the MMA8452Q must be disabled using the active()
method.

\param enable Whether to enable or disable the auto-sleep mode.

Example:
\verbatim embed:rst
.. code-block:: c++

  accel.active(false);
  accel.autoSleep(true);
  accel.active(true);
\endverbatim
 */
void autoSleep(bool enable);

/*! Enable/disable the orientation detection.

Before calling this method, the MMA8452Q must be disabled using the active()
method.

\param enable Whether to enable or disable the orientation detection.

Example:
\verbatim embed:rst
.. code-block:: c++

  accel.active(false);
  accel.detectOrientation(true);
  accel.active(true);
\endverbatim
 */
void detectOrientation(bool enable);

/*! Enable/disable auto-wake on specific events.

Before calling this method, the MMA8452Q must be disabled using the active()
method.

\param enable Whether to enable or disable the auto-wake on the selected
events.
\param events Bit mask listing the desired events. See MMA8452Q::MMA8452Q_EVENTS.

Example:
\verbatim embed:rst
.. code-block:: c++

  accel.active(false);
  accel.wakeOn(true, FREEFALL_MOTION | ORIENTATION);
  accel.active(true);
\endverbatim
 */
void wakeOn(bool enable, uint8_t events);

/* void intDataRdy(bool enable, uint8_t pin); */
/* void intFreefallMotion(bool enable, uint8_t pin); */
/* void intPulse(bool enable, uint8_t pin); */
/* void intOrientation(bool enable, uint8_t pin); */
/* void intAutoSlp(bool enable, uint8_t pin); */
};

#endif
