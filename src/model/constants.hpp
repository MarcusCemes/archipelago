// archipelago - model/constants.hpp
// Global program constants

#ifndef MODEL_CONSTANTS_H
#define MODEL_CONSTANTS_H

/* === NODES === */

/** Initial town dimension */
constexpr double DIM_MAX(1000.);
/** Minimum safety distance for overlap detection */
constexpr double DIST_MIN(10.);

/** Minimum district population (quartier) */
constexpr unsigned MIN_CAPACITY(1e3);
/** Maximum district population (quartier) */
constexpr unsigned MAX_CAPACITY(1e6);

/** Maximum input file line length */
constexpr unsigned MAX_LINE(80);
/** Maximum housing district connections (logement) */
constexpr unsigned MAX_LINK(3);
/** Specifies a `null` value for link connections */
constexpr unsigned NO_LINK(static_cast<unsigned>(-1));

/* === TRAVEL === */

/** Travelling speed */
constexpr double DEFAULT_SPEED(5.);
/** Travelling speed between transport districts */
constexpr double FAST_SPEED(20.);
/** Constant for infinite travel time */
constexpr double INFINITE_TIME(1e100);

/* === GRAPHICAL INTERFACE === */

/** Initial canvas size (window size) */
constexpr unsigned DEFAULT_DRAWING_SIZE(800);

/* Zoom step (incrementation) */
constexpr double DELTA_ZOOM(0.2);
/** Minimum zoom setting */
constexpr double MAX_ZOOM(3.);
/** Maximum zoom setting */
constexpr double MIN_ZOOM(0.2);

#endif
