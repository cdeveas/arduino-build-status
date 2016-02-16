# arduino-build-status
Arduino code for fetching a simple JSON object from an endpoint containing information on the build status of one or more codebases.

This code was used on a SparkFun ESP8266 Thing board to light up to four light bulbs with the build status of different codebases: A failed test results in a positive output on a pin, which triggers a relay to light up a corresponding light bulb (alerting the developers to that failed codebase).

Note- a JSON parsing library was intentionally not used in to conserve memory on the lightweight ESP8266 chip.
