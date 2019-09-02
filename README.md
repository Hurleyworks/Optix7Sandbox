# Optix7Sandbox

An environment for experimenting with the new Optix7 API

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. See deployment for notes on how to deploy the project on a live system.

### Prerequisites

System Requirements (for running binaries referencing OptiX)
##Graphics Hardware:

● All NVIDIA GPUs of Compute Capability 5.0 (Maxwell) or higher are supported.
Graphics Driver:

● OptiX 7.0.0 requires that you install the 435.80 driver on Windows or the 435.12 Driver for linux.. Note
OptiX dll from the SDK are no longer needed since the symbols are loaded from the driver.

● Windows 7/8.1/10 64-bit; 

##CUDA Toolkit

● It is not required to have any CUDA toolkit installed to be able to run OptiX-based applications.
Development Environment Requirements (for compiling with OptiX)

● CUDA Toolkit 7, 8, 9, 10

OptiX 7.0.0 has been built with CUDA 10.1, but any specified toolkit should work when compiling PTX for OptiX.

OptiX uses the CUDA device API, but the CUDA runtime API objects can be cast to device API obj

```
Give examples
```

### Installing

A step by step series of examples that tell you how to get a development env running

Say what the step will be

```
Give the example
```

And repeat

```
until finished
```

End with an example of getting some data out of the system or using it for a little demo

## Running the tests

Explain how to run the automated tests for this system

### Break down into end to end tests

Explain what these tests test and why

```
Give an example
```

### And coding style tests

Explain what these tests test and why

```
Give an example
```

## Deployment

Add additional notes about how to deploy this on a live system

## Built With

* [Dropwizard](http://www.dropwizard.io/1.0.2/docs/) - The web framework used
* [Maven](https://maven.apache.org/) - Dependency Management
* [ROME](https://rometools.github.io/rome/) - Used to generate RSS Feeds

## Contributing

Please read [CONTRIBUTING.md](https://gist.github.com/PurpleBooth/b24679402957c63ec426) for details on our code of conduct, and the process for submitting pull requests to us.

## Versioning

We use [SemVer](http://semver.org/) for versioning. For the versions available, see the [tags on this repository](https://github.com/your/project/tags). 

## Authors

* **Billie Thompson** - *Initial work* - [PurpleBooth](https://github.com/PurpleBooth)

See also the list of [contributors](https://github.com/your/project/contributors) who participated in this project.

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details

## Acknowledgments

* Hat tip to anyone whose code was used
* Inspiration
* etc
