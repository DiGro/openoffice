/**************************************************************
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
 *************************************************************/

#ifndef _PARCSS1_HXX
#define _PARCSS1_HXX

#include <tools/string.hxx>

class Color;

// Die Tokens des CSS1-Parsers
enum CSS1Token
{
	CSS1_NULL,
	CSS1_UNKOWN,

	CSS1_IDENT,
	CSS1_STRING,
	CSS1_NUMBER,
	CSS1_PERCENTAGE,
	CSS1_LENGTH,			// eine absolute Größe in 1/100 mm
	CSS1_PIXLENGTH,			// eine Pixel-Größe
	CSS1_EMS,
	CSS1_EMX,
	CSS1_HEXCOLOR,

	CSS1_DOT_W_WS,
	CSS1_DOT_WO_WS,
	CSS1_COLON,
	CSS1_SLASH,
	CSS1_PLUS,
	CSS1_MINUS,
	CSS1_OBRACE,
	CSS1_CBRACE,
	CSS1_SEMICOLON,
	CSS1_COMMA,
	CSS1_HASH,

	CSS1_IMPORT_SYM,
// Feature: PrintExt
	CSS1_PAGE_SYM,
// /Feature: PrintExt

	CSS1_IMPORTANT_SYM,

	CSS1_URL,
	CSS1_RGB
};


// die Zustände des Parsers
enum CSS1ParserState
{
	CSS1_PAR_ACCEPTED = 0,
	CSS1_PAR_WORKING,
	CSS1_PAR_ERROR
};


enum CSS1SelectorType
{
	CSS1_SELTYPE_ELEMENT,
	CSS1_SELTYPE_ELEM_CLASS,
	CSS1_SELTYPE_CLASS,
	CSS1_SELTYPE_ID,
	CSS1_SELTYPE_PSEUDO,
// Feature: PrintExt
	CSS1_SELTYPE_PAGE
// /Feature: PrintExt

};

// Die folgende Klasse beschreibt einen Simple-Selector, also
// - einen HTML-Element-Namen
// - einen HTML-Element-Namen mit Klasse (durch '.' getrennt)
// - eine Klasse (ohne Punkt)
// - eine mit ID=xxx gesetzte ID aus einem HTML-Dokument
// oder
// - ein Pseudo-Element
//
// Die Simple-Sektoren werden in einer Liste zu vollständigen
// Selektoren verkettet
class CSS1Selector
{
	CSS1SelectorType eType;		// Art des Selektors
	String aSelector;			// der Selektor selbst
	CSS1Selector *pNext;		// die nächste Komponente

public:

	CSS1Selector( CSS1SelectorType eTyp, const String &rSel )
		: eType(eTyp), aSelector( rSel ), pNext( 0 )
	{}

	~CSS1Selector();

	CSS1SelectorType GetType() const { return eType; }
	const String& GetString() const { return aSelector; }

	void SetNext( CSS1Selector *pNxt ) { pNext = pNxt; }
	const CSS1Selector *GetNext() const { return pNext; }
};


// Die folgende Klasse beschreibt einen Teil-Ausdruck einer
// CSS1-Deklaration sie besteht aus
//
// - dem Typ des Ausdrucks (entspricht dem Token)
// - dem eigentlichen Wert als String und ggf. double
//   der double-Wert enthält das Vorzeichen für NUMBER und LENGTH
// - und dem Operator, mit dem er mit dem *Vorgänger*-Ausdruck
//   verknüpft ist.
//
struct CSS1Expression
{
	sal_Unicode cOp;		// Art der Verknüpfung mit dem Vorgänger
	CSS1Token eType;		// der Typ des Wertes
	String aValue;			// und sein Wert als String
	double nValue;			// und als Zahl (TWIPs für LENGTH)
	CSS1Expression *pNext;	// die nächste Komponente

public:

	CSS1Expression( CSS1Token eTyp, const String &rVal,
					double nVal, sal_Unicode cO = 0 )
		: cOp(cO), eType(eTyp), aValue(rVal), nValue(nVal), pNext(0)
	{}

	~CSS1Expression();

	inline void Set( CSS1Token eTyp, const String &rVal, double nVal,
					 sal_Unicode cO = 0 );

	CSS1Token GetType() const { return eType; }
	const String& GetString() const { return aValue; }
	double GetNumber() const { return nValue; }
	inline sal_uInt32 GetULength() const;
	inline sal_Int32 GetSLength() const;
	sal_Unicode GetOp() const { return cOp; }


	sal_Bool GetURL( String& rURL ) const;
	sal_Bool GetColor( Color &rRGB ) const;

	void SetNext( CSS1Expression *pNxt ) { pNext = pNxt; }
	const CSS1Expression *GetNext() const { return pNext; }
};

inline void CSS1Expression::Set( CSS1Token eTyp, const String &rVal,
								 double nVal, sal_Unicode cO )
{
	cOp = cO; eType = eTyp; aValue = rVal; nValue = nVal; pNext = 0;
}

inline sal_uInt32 CSS1Expression::GetULength() const
{
	return nValue < 0. ? 0UL : (sal_uInt32)(nValue + .5);
}

inline sal_Int32 CSS1Expression::GetSLength() const
{
	return (sal_Int32)(nValue + (nValue < 0. ? -.5 : .5 ));
}

// Diese Klasse parst den Inhalt eines Style-Elements oder eine Style-Option
// und bereitet ihn ein wenig auf.
//
// Das Ergebnis des Parsers wird durch die Methoden SelectorParsed()
// und DeclarationParsed() an abgeleitete Parser übergeben. Bsp:
//
// H1, H2 { font-weight: bold; text-align: right }
//  |  |				    | 				   |
//  |  |                    |                  DeclP( 'text-align', 'right' )
//  |  |				    DeclP( 'font-weight', 'bold' )
//  |  SelP( 'H2', sal_False )
//  SelP( 'H1', sal_True )
//
class CSS1Parser
{
	sal_Bool bWhiteSpace : 1;			// White-Space gelesen?
	sal_Bool bEOF : 1;					// Ende des "Files" ?

	sal_Unicode cNextCh;				// nächstes Zeichen

	xub_StrLen nInPos;					// aktuelle Position im Input-String

	sal_uInt32 nlLineNr;				// akt. Zeilen Nummer
	sal_uInt32 nlLinePos;				// akt. Spalten Nummer

	double nValue;					// der Wert des Tokens als Zahl

	CSS1ParserState eState;			// der aktuelle Zustand der Parsers
	CSS1Token nToken;				// das aktuelle Token

	String aIn;						// der zu parsende String
	String aToken;					// das Token als String

	// Parsen vorbereiten
	void InitRead( const String& rIn );

	// das nächste Zeichen holen
	sal_Unicode GetNextChar();

	// das nächste Token holen
	CSS1Token GetNextToken();

	// arbeitet der Parser noch?
	sal_Bool IsParserWorking() const { return CSS1_PAR_WORKING == eState; }

	sal_Bool IsEOF() const { return bEOF; }

	sal_uInt32 IncLineNr() { return ++nlLineNr; }
	sal_uInt32 IncLinePos() { return ++nlLinePos; }
	inline sal_uInt32 SetLineNr( sal_uInt32 nlNum );			// inline unten
	inline sal_uInt32 SetLinePos( sal_uInt32 nlPos );			// inline unten

	// Parsen von Teilen der Grammatik
	void ParseRule();
	CSS1Selector *ParseSelector();
	CSS1Expression *ParseDeclaration( String& rProperty );

protected:

	void ParseStyleSheet();

	// Den Inhalt eines HTML-Style-Elements parsen.
	// Für jeden Selektor und jede Deklaration wird
	// SelectorParsed() bzw. DeclarationParsed() aufgerufen.
	sal_Bool ParseStyleSheet( const String& rIn );

	// Den Inhalt einer HTML-Style-Option parsen.
	// Für jede Deklaration wird DeclarationParsed() aufgerufen.
	sal_Bool ParseStyleOption( const String& rIn );

	// Diese Methode wird aufgerufen, wenn ein Selektor geparsed wurde
	// Wenn 'bFirst' gesetzt ist, beginnt mit dem Selektor eine neue
	// Deklaration. Wird sal_True zurückgegeben, wird der Selektor
	// gelöscht, sonst nicht.
	// Die Implementierung dieser Methode gibt nur sal_True zurück.
	virtual sal_Bool SelectorParsed( const CSS1Selector *pSelector,
								 sal_Bool bFirst );

	// Diese Methode wird für jede geparste Property aufgerufen. Wird
	// sal_True zurückgegeben wird der Selektor gelöscht, sonst nicht.
	// Die Implementierung dieser Methode gibt nur sal_True zurück.
	virtual sal_Bool DeclarationParsed( const String& rProperty,
									const CSS1Expression *pExpr );

public:

	CSS1Parser();
	virtual ~CSS1Parser();

	inline sal_uInt32	GetLineNr() const		{ return nlLineNr; }
	inline sal_uInt32	GetLinePos() const		{ return nlLinePos; }
};

inline sal_uInt32 CSS1Parser::SetLineNr( sal_uInt32 nlNum )
{
	sal_uInt32 nlOld = nlLineNr;
	nlLineNr = nlNum;
	return nlOld;
}

inline sal_uInt32 CSS1Parser::SetLinePos( sal_uInt32 nlPos )
{
	sal_uInt32 nlOld = nlLinePos;
	nlLinePos = nlPos;
	return nlOld;
}


#endif

/* vim: set noet sw=4 ts=4: */
