# Planning the itinerary between 2 locations and reserving the tickets

- - -

The project should find several variants for a trip between 2 places, sorted by total travel distance, by trip duration or by price. The user might attempt to book any variant for multiple persons. Later cancellations for any given number of persons are allowed, as well.

### Current considerations and assumed limitations:

- a displayed variant doesn&#39;t check if all involved transportation means have *enough remaining available seats*. Only booking attempts will do that. This approach allows fast reports and avoids repeated availability tests
- tickets for *multiple persons* might be booked at once. When there are not enough seats available, the booking should fail and report the largest number of persons that can reserve that trip
- *air*, *rail*, *road* and *water* are the accepted transportation modes. For the sake of simplicity, any *transportation mode switch* during a trip involves also a *different provider*, with its own schedule (This avoids specifying the transportation mode between any 2 consecutive places)
- locations have a *unique GPS location*, *several names / aliases* and an (*optional*) *description* to help distinguish similar places. They also act as if *all of them belong to a single time zone*. This prevents obtaining occasional [negative trip durations](https://www.quora.com/Are-there-flights-that-land-before-they-leave)
- the distance between places (measured in **km**-s) is expressed as a *floating-point value*. The distance between any 2 (*consecutive*) stops is the same for all providers using the *same transportation mode* and covering those stops in succession (*without intermediary detours*)
- *daylight saving* is ignored. All times use the *24-hour format* (00:00 - 23:59) without seconds. Apart from the departure time, the timetable of a route might contain times larger than 24:00, to illustrate that the trip does&#39;t end during the same day
- trips can be planned *only within a year* from the moment of the query. The dates use the *NamedMonth-Day\[-Year\]* format. The dates which don&#39;t mention the year and refer to months before current month actually point to those months from the next year
- all transportation means are *operational during several days of the week* and separately they might specify various dates within the next 12 months when they don&#39;t operate (this list needs to be updated monthly)
- there is a fixed number of available seats for a transportation mean during its entire route. Trains, for instance, cannot add / remove wagons at stopovers
- ticket prices handle and display only values expressed in **$**. Pricing rules need to ensure profit even for short trips, so there is a larger fare per km for small distance routes. *Airplane fares* depend also on `urgency` (how soon one needs to fly) and `occupancy` (the percentage of occupied seats after the current booking of a number of seats within the desired class - business / economy). For large / small `urgency` or `occupancy` values, the fare is up to `highFareFactor` / `lowFareFactor` times larger / smaller than the **normal fare**

* * *

### Possible extensions:

- using *other currencies*, apart from the **$**
- accepting / displaying distances in **mile**s, not only in **km**-s
- using *daylight saving* and *any time zones*, not only GMT
- allowing the *update (increase / decrease) of the available seats* at stopovers for a transportation mean
- using *external resources* as input for the application
- *GUI / web* user interface

- - -

&copy; 2017 Florin Tulba (GNU AGPL v3 license)
