/*
 This file is part of MARS.

 MARS is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 MARS is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with MARS.  If not, see <http://www.gnu.org/licenses/>.
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
	{ "Load slot ",		"Carica slot "			},
	{ "Save slot ",		"Salva slot "			},
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

Language ChineseSimplified = {
	{ "File",			"文件"					},
	{ "Emulation",		"模拟"					},
	{ "Language",		"语言"					},
	{ "Help",			"帮助"					},
	{ "Open",			"打开"					},
	{ "Open Recent",	"最近打开"				},
	{ "Load State",		"载入状态"				},
	{ "Save State",		"保存状态"				},
	{ "Load slot ",		"载入插槽 "				},
	{ "Save slot ",		"保存插槽 "				},
	{ "Exit",			"退出"					},
	{ "Settings",		"设置"					},
	{ "Play",			"播放"					},
	{ "Pause",			"暂停"					},
	{ "Stop",			"停止"					},
	{ "Debug",			"调试"					},
	{ "Fullscreen",		"全屏"					},
	{ "About",			"关于"					},
	{ "Recent",			"最近"					},
	{ "Switch between 2D and 3D",		"在2D和3D之间切换" },
	{ "Refresh",		"刷新"					},
	{ "Error",			"错误"					},
	{ "OK",				"确定"					}
};

Language ChineseTraditional = {
	{ "File",			"檔案"					},
	{ "Emulation",		"模擬"					},
	{ "Language",		"語言"					},
	{ "Help",			"幫助"					},
	{ "Open",			"開啟"					},
	{ "Open Recent",	"最近開啟"				},
	{ "Load State",		"載入狀態"				},
	{ "Save State",		"儲存狀態"				},
	{ "Load slot ",		"載入插槽 "				},
	{ "Save slot ",		"儲存插槽 "				},
	{ "Exit",			"退出"					},
	{ "Settings",		"設置"					},
	{ "Play",			"播放"					},
	{ "Pause",			"暫停"					},
	{ "Stop",			"停止"					},
	{ "Debug",			"調試"					},
	{ "Fullscreen",		"全螢幕"					},
	{ "About",			"關於"					},
	{ "Recent",			"最近"					},
	{ "Switch between 2D and 3D", "在2D和3D之間切換" },
	{ "Refresh",		"刷新"					},
	{ "Error",			"錯誤"					},
	{ "OK",				"確定"					}
};

Language Japanese = {
	{ "File",			"ファイル"				},
	{ "Emulation",		"エミュレーション"			},
	{ "Language",		"言語"					},
	{ "Help",			"ヘルプ"					},
	{ "Open",			"開く"					},
	{ "Open Recent",	"最近開いた"				},
	{ "Load State",		"状態を読み込む"			},
	{ "Save State",		"状態を保存する"			},
	{ "Load slot ",		"スロットを読み込む "		},
	{ "Save slot ",		"スロットを保存する "		},
	{ "Exit",			"終了"					},
	{ "Settings",		"設定"					},
	{ "Play",			"再生"					},
	{ "Pause",			"一時停止"				},
	{ "Stop",			"停止"					},
	{ "Debug",			"デバッグ"				},
	{ "Fullscreen",		"フルスクリーン"			},
	{ "About",			"概要"					},
	{ "Recent",			"最近の"					},
	{ "Switch between 2D and 3D", "2Dと3Dを切り替える" },
	{ "Refresh",		"リフレッシュ"			},
	{ "Error",			"エラー"					},
	{ "OK",				"OK"					}
};

Language Korean = {
	{ "File",			"파일"					},
	{ "Emulation",		"에뮬레이션"				},
	{ "Language",		"언어"					},
	{ "Help",			"도움말"					},
	{ "Open",			"열기"					},
	{ "Open Recent",	"최근 열기"				},
	{ "Load State",		"상태 불러오기"			},
	{ "Save State",		"상태 저장하기"			},
	{ "Load slot ",		"슬롯 불러오기 "			},
	{ "Save slot ",		"슬롯 저장하기 "			},
	{ "Exit",			"종료"					},
	{ "Settings",		"설정"					},
	{ "Play",			"재생"					},
	{ "Pause",			"일시 정지"				},
	{ "Stop",			"정지"					},
	{ "Debug",			"디버그"					},
	{ "Fullscreen",		"전체 화면"				},
	{ "About",			"정보"					},
	{ "Recent",			"최근"					},
	{ "Switch between 2D and 3D", "2D와 3D 전환" },
	{ "Refresh",		"새로 고침"				},
	{ "Error",			"오류"					},
	{ "OK",				"확인"					}
};

std::map<std::string_view, Language> _languages = {
	{ "de",		German	},
	{ "el",		Greek	},
	{ "es",		Spanish },
	{ "fr",		French	},
	{ "it",		Italian },
	{ "zh_CN",	ChineseSimplified },
	{ "zh_TW",	ChineseTraditional },
	{ "ja",		Japanese },
	{ "ko",		Korean  },
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
