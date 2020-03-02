// constantes.h - Global constants


/* NOEUDS */

/**  Délimiation de l'espace initiale de la ville */
constexpr double dim_max(1000.);
/** Distance de securite pour les tests de superposition */
constexpr double dist_min(10.);

/** Nombre minimum de personnes */
constexpr unsigned min_capacity(1e3);
/** Nombre maximum de personnes */
constexpr unsigned max_capacity(1e6);

/** Nombre de caractères maximum par ligne lors de la lecture d'un fichier */
constexpr unsigned max_line(80);
/** Nombre maximum de connection d'un Logement */
constexpr unsigned max_link(3);
/** Exprime un lien qui n'existe pas */
constexpr unsigned no_link(static_cast<unsigned>(-1));


/* DEPLACEMENT */

/** Vitesse de déplacement */
constexpr double default_speed(5.);
/** Vitesse de déplacement entre deux noeuds Transport */
constexpr double fast_speed(20.);
/** Valeur qui représente un temps infini */
constexpr double infinite_time(1e100);


/* INTERFACE GRAPHIQUE */

/** Taille initialie du canvas (de la fenêtre) */
constexpr unsigned default_drawing_size(800);
/* Facteur d'incrémentation de zoom */
constexpr double delta_zoom(0.2);
/** Facteur de zoom maximal */
constexpr double max_zoom(3.);
/** Facteur de zoom minimal */
constexpr double min_zoom(0.2);