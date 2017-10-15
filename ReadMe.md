# Planning the itinerary between 2 locations and reserving the tickets

- - -

The project should find several variants for a trip between 2 places, sorted by total travel distance, by trip duration or by price. The user might attempt to book any variant for multiple persons. Later cancellations for any given number of persons are allowed, as well.

### Current considerations and assumed limitations:

- a displayed variant doesn&#39;t check if all involved transportation means have *enough remaining available seats*. Only booking attempts will do that. This approach allows fast reports and avoids repeated availability tests
- tickets for *multiple persons* might be booked at once. When there are not enough seats available, the booking should fail and report the largest number of persons that can reserve that trip
- *air*, *rail*, *road* and *water* are the accepted transportation modes 
- locations have a *unique name* and they act as if *all of them belong to a single time zone*. This prevents obtaining occasional [negative trip durations](https://www.quora.com/Are-there-flights-that-land-before-they-leave)
- the distance between places (measured in **km**-s) is expressed as a *floating-point value*
- *daylight saving* is ignored. All times use the *24-hour format* (00:00 - 23:59) without seconds
- trips can be planned *only within a year* from the moment of the query. All dates use *DD.MM* format and *don&#39;t use the year*. The dates referring to months before current month actually point to those months from the next year
- all transportation means are *operational during several days of the week* and separately they might specify various dates within the next 11 months when they don&#39;t operate (this list needs to be updated monthly)
- there is a fixed number of available seats for a transportation mean during its entire route. Trains, for instance, cannot add / remove wagons at stopovers
- ticket prices handle and display only values expressed in **$**. Pricing rules need to ensure profit even for short trips, so there is a larger fare per km for small distance routes. This is modelled through a **normal fare** rule: `ln(1 + k * tripDistance)`, where `k` is an adjustable constant, set / updated by the transportation provider. Additionaly:
	- *Business-class* (first class) tickets use a larger `k` than *economy-class* (second class)
	- *Airplane fares* depend also on `urgency` (how soon one needs to fly; value 1 means today, value 0 is a year from now) and `occupancy` (the percentage of occupied seats after the current booking of a number of seats within the desired class - business / economy). For large / small `urgency` or `occupancy` values, the fare is up to `highFareFactor` / `lowFareFactor` times larger / smaller than the **normal fare**. So, the used formula is: `ln(1 + k * tripDistance) * (lowFareFactor + (highFareFactor - lowFareFactor) * (e^max(urgency, occupancy) - 1) / (e - 1))`

* * *

### Possible extensions:

- using several *alias names* for some locations
- allowing *multiple locations with the same name / alias*. Their *GPS location* should help distinguish them
- displaying the appropriate alias, acording to the *locale*
- using *other currencies*, apart from the **$**
- accepting / displaying distances in **mile**s, not only in **km**-s
- using *daylight saving* and *any time zones*, not only GMT
- allowing the *update (increase / decrease) of the available seats* at stopovers for a transportation mean
- using *external resources* as input for the application
- *GUI / web* user interface

- - -

&copy; 2017 Florin Tulba (GNU AGPL v3 license)
