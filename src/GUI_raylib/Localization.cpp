/*
 This file is part of MultiEmu3D.

 MultiEmu3D is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 MultiEmu3D is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with MultiEmu3D.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <map>
#include "Localization.h"

typedef std::map<std::string_view, std::string_view> Language;

Language Spanish = {
	{ "File",			"Archivo"				},
	{ "Emulation",		"Emulación"				},
	{ "Language",		"Idioma"				},
	{ "Help",			"Ayuda"					},
	{ "Open",			"Abrir"					},
	{ "Open Recent",	"Abrir recientes"	},
	{ "Load State",		"Cargar estado"			},
	{ "Save State",		"Guardar estado"		},
	{ "Load slot ",		"Cargar ranura "		},
	{ "Save slot ",		"Guardar raura "		},
	{ "Exit",			"Salir"					},
	{ "Settings",		"Configuración"			},
	{ "Play",			"Jugar"					},
	{ "Pause",			"Pausar"				},
	{ "Stop",			"Parar"					},
	{ "Debug",			"Depurar"				},
	{ "Fullscreen",		"Pantalla completa"		},
	{ "About",			"Acerca de"				},
	{ "Recent",			"Reciente"				},
	{ "Switch between 2D and 3D", "Cambiar entre 2D y 3D" },
	{ "Refresh",		"Refresco"				},
	{ "Error",			"Error"					},
	{ "OK",				"OK"					}
};

Language Greek = {
	{ "File",			"Αρχείο"				},
	{ "Emulation",		"Εξομοίωση"				},
	{ "Language",		"Γλώσσα"				},
	{ "Help",			"Βοήθεια"				},
	{ "Open",			"Άνοιγμα"				},
	{ "Open Recent",	"Άνοιγμα Πρόσφατων"	},
	{ "Load State",		"Φόρτωση Κατάστασης"	},
	{ "Save State",		"Αποθήκευση Κατάστασης" },
	{ "Load slot ",		"Φόρτωση υποδοχής "		},
	{ "Save slot ",		"Αποθήκευση υποδοχής "	},
	{ "Exit",			"Έξοδος"				},
	{ "Settings",		"Ρυθμίσεις"				},
	{ "Play",			"Παίξτε"				},
	{ "Pause",			"Παύση"					},
	{ "Stop",			"Διακοπή"				},
	{ "Debug",			"Αποσφαλμάτωση"			},
	{ "Fullscreen",		"Πλήρης-οθόνη"			},
	{ "About",			"Σχετικά"				},
	{ "Recent",			"Πρόσφατα"				},
	{ "Switch between 2D and 3D", "Εναλλαγή μεταξύ 2D και 3D" },
	{ "Refresh",		"Ανανέωση"				},
	{ "Error",			"Σφάλμα"				},
	{ "OK",				"OK"					}
};

Language French = {
	{ "File",			"Fichier"				},
	{ "Emulation",		"Émulation"				},
	{ "Language",		"Langue"				},
	{ "Help",			"Aide"					},
	{ "Open",			"Ouvrir"				},
	{ "Open Recent",	"Ouvrir Récemment"		},
	{ "Load State",		"Charger l'état"		},
	{ "Save State",		"Sauvegarder l'état"	},
	{ "Load slot ",		"Charger l'emplacement " },
	{ "Save slot ",		"Sauvegarder l'emplacement "},
	{ "Exit",			"Sortie"				},
	{ "Settings",		"Réglages"				},
	{ "Play",			"Jouer"					},
	{ "Pause",			"Pause"					},
	{ "Stop",			"Arrêter"				},
	{ "Debug",			"Déboguer"				},
	{ "Fullscreen",		"Plein écran"			},
	{ "About",			"À propos de"			},
	{ "Recent",			"Récent"				},
	{ "Switch between 2D and 3D", "Passer de la 2D à la 3D" },
	{ "Refresh",		"Rafraîchissement"		},
	{ "Error",			"Erreur"				},
	{ "OK",				"OK"					}
};

Language German = {
	{ "File",			"Datei"					},
	{ "Emulation",		"Emulation"				},
	{ "Language",		"Sprache"				},
	{ "Help",			"Hilfe"					},
	{ "Open",			"Öffnen"				},
	{ "Open Recent",	"Zuletzt geöffnet"		},
	{ "Load State",		"Zustand laden"			},
	{ "Save State",		"Zustand speichern"		},
	{ "Load slot ",		"Slot laden "			},
	{ "Save slot ",		"Slot speichern "		},
	{ "Exit",			"Beenden"				},
	{ "Settings",		"Einstellungen"			},
	{ "Play",			"Abspielen"				},
	{ "Pause",			"Pause"					},
	{ "Stop",			"Stoppen"				},
	{ "Debug",			"Debuggen"				},
	{ "Fullscreen",		"Vollbild"				},
	{ "About",			"Über"					},
	{ "Recent",			"Neueste"				},
	{ "Switch between 2D and 3D", "Zwischen 2D und 3D wechseln" },
	{ "Refresh",		"Aktualisieren"			},
	{ "Error",			"Fehler"				},
	{ "OK",				"OK"					}
};

Language Italian = {
	{ "File",			"File"					},
	{ "Emulation",		"Emulazione"			},
	{ "Language",		"Lingua"				},
	{ "Help",			"Aiuto"					},
	{ "Open",			"Apri"					},
	{ "Open Recent",	"Apri recenti"			},
	{ "Load State",		"Carica stato"			},
	{ "Save State",		"Salva stato"			},
	{ "Load slot ",		"Carica slot"			},
	{ "Save slot ",		"Salva slot"			},
	{ "Exit",			"Uscita"				},
	{ "Settings",		"Impostazioni"			},
	{ "Play",			"Riproduci"				},
	{ "Pause",			"Pausa"					},
	{ "Stop",			"Stop"					},
	{ "Debug",			"Debug"					},
	{ "Fullscreen",		"Schermo intero"		},
	{ "About",			"Informazioni"			},
	{ "Recent",			"Recenti"				},
	{ "Switch between 2D and 3D", "Cambia tra 2D e 3D" },
	{ "Refresh",		"Aggiorna"				},
	{ "Error",			"Errore"				},
	{ "OK",				"OK"					}
};

std::map<std::string_view, Language> _languages = {
	{ "de", German	},
	{ "el", Greek	},
	{ "es", Spanish },
	{ "fr", French	},
	{ "it", Italian }
};

Language* _selectedLanguage = nullptr;

bool Localization::IsAvailable(std::string_view language) {
	if (_languages.find(language) != _languages.end())
		return true;
	else
		return false;
}

bool Localization::SetLanguage(std::string_view language) {
	std::map<std::string_view, Language>::iterator it = _languages.find(language);
	if (it != _languages.end()) {
		_selectedLanguage = &it->second;
		return true;
	}
	else {
		_selectedLanguage = nullptr;
		return false;
	}
}

std::string_view Localization::TranslateStrView(std::string_view text) {
	if (_selectedLanguage != nullptr) {
		Language::iterator it = _selectedLanguage->find(text);
		if (it != _selectedLanguage->end())
			return it->second;
		else
			return text;
	}
	else
		return text;
}

const char* Localization::Translate(std::string_view text) {
	return TranslateStrView(text).data();
}
