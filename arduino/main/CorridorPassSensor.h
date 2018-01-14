/**
 * A trivial class which reads data from two proximity sensors
 * and evaluates which way an object went, it also tries to
 * recover from an inconsistent state.
 *
 * This is a blocking class - it is reading the values of the
 * sensors all the time, looking for a change - approaching
 * this with interrupts "sucked" as ISRs cannot be interrupted
 * and without timer interrupts it gets really cumbersome...
 *
 * There are following events which can have a void(void)
 * handler function attached:
 *
 *    onForward  - triggered when the object crossed the first
 *                 sensor and then the second
 *
 *    onBackward - triggered when the object crossed the second
 *                 sensor and only then the fist one
 *
 *    onError    - when the state of sensors is inconsistent,
 *                 e.g. when a sensor doesn't revert to its
 *                 default state (misalligned, unplugged...)
 *
 *    onReset  - when internal counters are inconsistent for
 *                 some time
 *
 */

// include only once
#ifndef CorridorPassSensor_h
#define CorridorPassSensor_h

// times in milliseconds
#define DEFAULT_ISSUE_ERROR_TIME 1000
#define DEFAULT_RESET_TIME 1000

#define NOISE_TIMEOUT 20

#define BLINK_DELAY_ON_ERROR 1000
#define WAIT_FOR_RESET_AFTER_NO_ERROR 5000

typedef void (*eventHandler)(void);

class CorridorPassSensor {

  public:
    CorridorPassSensor(int  sensor1_pin, int  sensor2_pin,
                       bool sensor1_polarity = 1, bool sensor2_polarity = 1,
                       eventHandler onForward  = nullptr,
                       eventHandler onBackward = nullptr,
                       eventHandler onError    = nullptr,
                       eventHandler onReset    = nullptr,
                       unsigned long issueErrorAfter = DEFAULT_ISSUE_ERROR_TIME,
                       unsigned long resetAfter = DEFAULT_RESET_TIME
        );

    // starts/stops the sensing
    void start();
    void stop();

    void onForward (eventHandler handler);
    void onBackward(eventHandler handler);
    void onError   (eventHandler handler);
    void onReset   (eventHandler handler);

    void setIssueErrorTime(unsigned long time);
    void setResetTime(unsigned long time);

  protected:
    void s1Triggered(unsigned long t);
    void s2Triggered(unsigned long t);
    unsigned long trig_ts_of_S1;
    unsigned long trig_ts_of_S2;

    void onForward ();
    void onBackward();
    void onError   ();
    void onReset   ();

  private:
    int pinS1, pinS2;
    bool polS1, polS2;

    eventHandler onForwardHandler, onBackwardHandler,
                 onErrorHandler, onResetHandler;

    unsigned long issueErrorTime, resetTime;

    bool started, inErrorState;
};

#endif
