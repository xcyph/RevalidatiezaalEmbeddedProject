# Revalidatiezaal EmbeddedProject
Embedded Project Groep D

# Huisstijl code – design patterns

**NOTE**: Maak je gebruik van een omgeving die zijn eigen notatie gebruikt? Maak dan gebruik van deze notatie. Een voorbeeld hiervan is de default code notatie van bijv. variabelenamen die in de **STM IDE** omgeving wordt gebruikt (zie voorbeeld hieronder).

```cpp
static void MX_I2C1_Init(void);

void SystemClock_Config(void)
{
    // ... 
}
```

## 1. Classnamen

**Namenstijl**: Gebruik PascalCase voor classnamen, waarbij elk woord begint met een hoofdletter.

* **Voorbeeld**: `MijnKlasse`, `GebruikersInvoer`, `DatabaseConnectie`.

## 2. Variabelenamen

**Namenstijl**: Gebruik camelCase voor variabelenamen.

* **Voorbeeld**: `mijnVariabele`, `tellerWaarde`, `gebruikersInvoer`.

**Betekenisvolle namen**: Geef variabelenamen betekenisvolle namen die de functie van de variabele beschrijven.

* **Goed**: `aantalStudenten`, `gebruikersNaam`, `totaalBedrag`.
* **Vermijd**: `x`, `temp`, `abc`.

## 3. Functienamen

**Namenstijl**: Gebruik camelCase voor functienamen.

* **Voorbeeld**: `berekenTotaal`, `valideerInvoer`, `zoekElement`.

**Haakjesstijl**: Plaats de haakjes op een nieuwe regel onder de functienaam voor de parent functie, en op dezelfde regel naast de functienaam voor child functies.

```cpp
void parentFunctie(int parameter1, double parameter2)
{
    // Parent functie implementatie

    void subFunctie() {
        // Child functie implementatie
    }
}
```

## 4. Codeopmaak

**Inspringing**: Gebruik een inspringing van 4 spaties.

**Leesbaarheid**: Houd de code leesbaar door voldoende witruimte te gebruiken en logische blokken te scheiden.

## 5. Commentaar

**Blok commentaar**: Begin commentaar boven codeblokken met hoofdletters op de volgende manier:

```cpp
/**
  * @brief Commentaar boven de codeblokken zien er op deze manier uit
  * en beginnen met een hoofdletter.
  */
```

**Inline commentaar**: Gebruik kleine letters voor inline commentaar naast de code.

```cpp
int voorbeeld = 10; /* inline commentaar zonder hoofdletter. */
```

## 6. Documentatie

**Doxygen format**: Documenteer code met behulp van Doxygen stijlcommentaar (zie voorbeeld hieronder).

```cpp
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
```

**Beschrijving**: Zorg voor duidelijke en beknopte beschrijvingen van functies en variabelen, inclusief hun doel, parameters en teruggegeven waarden.

**Doxygen-tags**: Gebruik de volgende Doxygen-tags voor verschillende elementen:

* `@brief`: Korte beschrijving van de functie/variabele.
* `@param`: Beschrijving van een functieparameter.
* `@return`: Beschrijving van de teruggegeven waarde.
* `@author`: Naam van de auteur.
* `@date`: Datum van laatste wijziging.
* `@version`: Versienummer.
* `@note`: Opmerking over de functie/variabele.
* `@warning`: Waarschuwing over de functie/variabele.
* `@todo`: Taak die nog moet worden voltooid.
* `@deprecated`: Beschrijving van waarom het element verouderd is.

## 7. Bestandsindeling

**Bestandsnaamconventie**: Gebruik kleine letters en underscores voor bestandsnamen.

* **Voorbeeld**: `mijn_bestand.cpp`, `hulpprogramma_functies.h`.
