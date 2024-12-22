#include <iostream>
#include <string>
#include <list>
#include <memory>


class ElectronicDevice {
protected:
    std::string brand;
    std::string model;

public:
    ElectronicDevice(const std::string& brand, const std::string& model)
        : brand(brand), model(model) {}

    virtual ~ElectronicDevice() = default;

    virtual void display() const {
        std::cout << "Brand: " << brand << ", Model: " << model << std::endl;
    }
};


class Smartphone : public ElectronicDevice {
private:
    std::string operatingSystem;

public:
    Smartphone(const std::string& brand, const std::string& model, const std::string& operatingSystem)
        : ElectronicDevice(brand, model), operatingSystem(operatingSystem) {}

    void display() const override {
        std::cout << "Smartphone - Brand: " << brand << ", Model: " << model
                  << ", OS: " << operatingSystem << std::endl;
    }
};

class Laptop : public ElectronicDevice {
private:
    double screenSize;

public:
    Laptop(const std::string& brand, const std::string& model, double screenSize)
        : ElectronicDevice(brand, model), screenSize(screenSize) {}

    void display() const override {
        std::cout << "Laptop - Brand: " << brand << ", Model: " << model
                  << ", Screen Size: " << screenSize << " inches" << std::endl;
    }
};


void modifyDevice(ElectronicDevice* device) {
    std::cout << "Modifying device...\n";
    device->display();
    std::cout << "Modification complete.\n";
}

int main() {
    std::list<std::shared_ptr<ElectronicDevice>> devices;

    devices.push_back(std::make_shared<Smartphone>("Apple", "iPhone 13", "iOS"));
    devices.push_back(std::make_shared<Laptop>("Dell", "XPS 15", 15.6));
    devices.push_back(std::make_shared<Smartphone>("Samsung", "Galaxy S22", "Android"));
    devices.push_back(std::make_shared<Laptop>("HP", "Pavilion", 14.0));

    std::cout << "Device List:\n";
    for (const auto& device : devices) {
        device->display();
    }

    std::cout << "\nModifying first device in the list:\n";
    if (!devices.empty()) {
        modifyDevice(devices.front().get());
    }

    return 0;
}
