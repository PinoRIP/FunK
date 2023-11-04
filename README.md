# FunK
FunK is a functional testing framework with multiplayer support for Unreal Engine 5. This is the plugin. The UE project used for development is available here: [FunKProject](https://github.com/PinoRIP/FunKProject).

## Contents

1. [Supported Unreal Engine Versions](#supported-unreal-engine-versions)
2. [Quick Start](#quick-start)
3. [Settings](#settings)
   1. [Test Discovery](#test-discovery)
4. [Tests](#tests)
   1. [Functional Test](#functional-test)
   2. [Stages](#stages)
   3. [Assertions](#assertions)
   4. [Variations](#variations)
   5. [Fragments](#fragments)
   6. [Input Simulation](#input-simulation)
   7. [WaitFor Synchronization Point](#waitfor-synchronization-point)
   8. [WaitFor Trigger](#waitfor-trigger)
5. [Run Tests](#run-tests)
6. [Technical Overview](#technical-overview)
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
The FunK functional testing framework can be configured but is usable right out of the box.

The settings are located under:
Edit > Project Settings > Plugins > FunK Settings.

### Test Discovery
There are several methods that can be used for test discovery.

1. **Search:** Every map in the project will be considered when discovering tests. This should generally be avoided.
2. **Prefix:** Every map with the configured prefix will be considered when discovering tests. This is the way Unreal Engine's built-in functional testing framework discovers tests, and the plugin defaults to it.
3. **Worlds:** Every map that should be considered when discovering tests has to be configured explicitly.
4. **Paths:** Every map that is placed under the configured paths is considered when discovering tests. This is the approach used by the [FunKProject](https://github.com/PinoRIP/FunKProject).

## Tests
The FunK tests are primarily built to be used in blueprints.

### Functional Test
To create a FunK functional test, follow these steps:

1. Create a new level.
2. Create a test actor blueprint by inheriting from `FunKFunctionalTest`.
3. In the test actor's event graph, implement the [stage](#stages) events:
   1. **Arrange:** This stage prepares everything needed for the actual test. Arrange is [optional](#optional) and [latent](#latent).
   2. **Act:** This stage performs the actual testing logic. Act is [latent](#latent).
   3. **Assert:** This stage verifies the results of the test. Assert is [optional](#optional) and can be made [latent](#latent) if needed.
4. Add an instance of the test actor blueprint to the level and save.

### Stages
Tests in the FunK functional testing framework are structured into stages. Each stage must be completed on every [network peer](#network-peer) before the next stage begins.

Stages can be categorized as follows:

#### Optional
The stage will finish successfully if the blueprint event is not implemented.

#### Latent
The stage can wait for asynchronous operations but must call `FinishStage` before a configurable time limit is reached. Latent stages can also have their own tick function that is only ticking when the stage is active.

### Assertions
The FunK functional testing framework provides an assertion library.

Please note that you can't use just any assertion framework, as assertions must be networked. However, you can create your own based on the `RaiseEvent` function defined in the `FunKTestBase`.

### Variations
The FunK functional testing framework provides a way to repeat the same test with slight variations. This is helpful to test with different network latencies and/or for different behavior based on the network roles (as in any normal client vs. the hosting client when using listen server).

Variations are provided by actor components derived from `FunKTestVariationComponent`. These can be added to a test. One variation component can be responsible for multiple variations. The actual variation is implemented as a [test fragment](#fragments).

#### Shared Variations
The FunK functional testing framework provides a way to share variations for a level by using a `FunKTestVariationsWorldActor`.

1. Create an actor by inheriting from `FunKTestVariationsWorldActor`.
2. Add the variations that should be available to every test in the world as components.
3. Add an instance of the actor to the level.

Please note that root variations can't be shared.

#### Root Variations
The FunK functional testing framework provides a special type of variation, the "root variation". A root variation can only be added to a test directly. Root variations differ from normal variations in that every root variation gets executed for every variation. Meaning when there are 3 variations and 3 root variations, the test will be executed 9 times.

Please note that every test can only have one root variation.

#### Actor Scenario Variation
The FunK functional testing framework provides a variation component to deal with actors. This has multiple reasons:

1. Since a test can be executed multiple times using variations, the referenced actors have to be reset or spawned and deleted after each test run.
2. The ownership of actors can create a need to repeat the test in order to check for consistent behavior between the host and connected clients.

Please note that the actor scenario variation is a root variation.

#### Network Emulation Variation
The FunK functional testing framework provides a variation component to test different networking conditions. It is possible to use the project's presets or custom network emulation settings.

### Fragments
The FunK functional testing framework provides "fragments" to share code between tests.

### Input Simulation
The FunK functional testing framework provides an input simulation library. This library supports the old input system, the enhanced input system, and raw key inputs.

Please note that the input simulation system also disables all actual player input while the tests are running.

### WaitFor Synchronization Point
The FunK functional testing framework provides a way for [latent](#latent) [stages](#stages) to synchronize tests with all not finished [network peers](#network-peer).

### WaitFor Trigger
The FunK functional testing framework provides a way for [latent](#latent) [stages](#stages) to wait until a trigger has been triggered.

## Run Tests
Every test that has been discovered by the FunK functional testing framework will be available in the 'Session Frontend' (Tools > Session Frontend > Automation > FunK). Here, the tests will be grouped by 'dedicated' / 'listen' / 'standalone' and then by map.

## Technical Overview
For a more technical overview of the whole framework, see [Technical Overview](https://github.com/PinoRIP/FunKProject).

## Naming

### Network Peer
Any participant (Server and every client) is considered a network peer. For example, in a scenario with one Server and two clients, there would be three network peers.

## Future Plans
These are not necessarily in order:
- Gauntlet & CI
- Input recording, so that input can be manually played and then repeated as a test
- Performance Tests (Including network usage etc.)