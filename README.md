# FunK
FunK is a functional testing framework with multiplayer support for Unreal Engine 5. This is the plugin. The UE-project used to develop is here: [FunKProject](https://github.com/PinoRIP/FunKProject)

## Contents

1. [Supported Unreal Engine Versions](#supported-unreal-engine-versions)
2. [Quick Start](#quick-start)
3. [Settings](#settings)
   1. [Test Discovery](#test-discovery)
4. [Tests](#tests)
   1. [Functional test](#functional-test)
   2. [Stages](#stages)
   3. [Assertions](#assertion)
   4. [Variations](#variations)
   5. [Fragments](#fragments)
   6. [Input Simulation](#input-simulation)
   7. [WaitFor synchronization point](#waitfor-synchronization-point)
   8. [WaitFor trigger](#waitfor-trigger)
5. [Run Tests](#run-tests)
6. [Technical overview](#technical-overview)
7. [Naming](#naming)

## Supported Unreal Engine Versions
The FunK functional testing framework is built and tested on Unreal Engine 5.1 but should work with any 5.x version.

## Quick Start
Please note that you currently have to compile the plugin yourself.
1. Clone the repository to your project's `Plugins` folder.
2. Close the Unreal Editor.
3. Recompile your project.
4. Start the Unreal Editor.
5. Enable the plugin in Edit > Plugins > FunK and restart (if needed).

## Settings
The FunK functional testing framework provides some basic configuration but is usable right out of the box.

The settings are located under:
Edit > Project Settings > Plugins > FunK Settings

### Test Discovery
There are several discovery methods that can be used to let the framework search the project for functional tests.
1. Search: Every map in the project will be considered when discovering tests. This should generally be avoided.
2. Prefix: Every map with the configured prefix will be considered when discovering tests. This is the way Unreal Engine's built-in functional testing framework discovers tests, and the plugin defaults to it.
3. Worlds: Every map to be considered when discovering tests can be configured explicitly.
4. Paths: Every map that is placed under the configured paths is considered when discovering tests. This is the way the [FunKProject](https://github.com/PinoRIP/FunKProject) uses.

## Tests
The FunK tests are primarily build to be used in blueprints.

### Functional test
To create a FunK functional test, follow these steps:
1. Create a new level.
2. Create a test actor blueprint by inheriting from `FunKFunctionalTest`.
3. In the test actors event graph implement the [stage](#stages) events:
   1. Arrange: This stage prepares everything needed for the actual test. Arrange is [optional](#optional) and [latent](#latent),
   2. Act: This stage performs the actual testing logic. Act is [latent](#latent),
   3. Assert: This stage verifies the results of the test. Assert is [optional](#optional) and can be made [latent](#latent) if needed.
4. Add an instance of the test actor blueprint to the level.

### Stages
Tests in the FunK functional testing framework are structured into stages. Each stage must be completed on every [network peer](#network-peer) before the next stage begins.

Stages can be categorized as follows:
#### Optional
The stage will finish successfully given the blueprint event is not implemented.

#### Latent
The stage can wait for asynchronous operations but must call `FinishStage` before a configurable time limit is reached. Latent stages can also have their own tick function that is only ticking when the stage is active.

### Assertions
The FunK functional testing framework provides an assertion blueprint function library.

Please note that you can't use just any assertion framework, as assertions must be networked. However, you can create your own based on the `RaiseEvent` function defined in the `FunKTestBase`.

### Variations

### Fragments

### Input Simulation

### WaitFor synchronization point

### WaitFor trigger

## Run Tests

## Technical overview

## Naming

### Network peer
Any participant (Server and every client) is considered a network peer. For example, in a scenario with one Server and two clients, there would be three network peers.


## Future plans
These are not necessarily in order:
- Readme.md with setup, usage & concepts
- Input simulation
- Utilities to help test with lag, relevancy & other multiplayer pitfalls
- Gauntlet & CI
- Input recording, so that input can be manually played and then repeated as a test
- Performance Tests
- Comments
