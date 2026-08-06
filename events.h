#ifndef EVENTS_H_
#define EVENTS_H_

#include <stdbool.h>
#include "types.h"


void initializeEventCards(EventCard cards[CARD_COUNT]);

void initializeEventDeck(EventDeck *deck);

void shuffleEventDeck(EventDeck *deck);

const EventCard *drawEventCard(EventDeck *deck, const EventCard cards[CARD_COUNT]);

bool validateEventCards(const EventCard cards[CARD_COUNT]);

bool validateEventDeck(const EventDeck *deck);

void printEventCard(const EventCard *card);


#endif