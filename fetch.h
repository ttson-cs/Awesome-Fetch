#ifndef FETCH_H
#define FETCH_H

#include <fstream>
#include <iostream>
#include <string>
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include <unordered_map>

class Fetch {
private:
  [[nodiscard]] auto getUser() -> std::string {
    return std::string{getenv("USER")};
  }

  [[nodiscard]] auto getHost() -> std::string {
    struct utsname name;
    uname(&name);
    return std::string{name.nodename};
  }

  [[nodiscard]] auto getOS() -> std::string {
    std::ifstream osRelease{"/etc/os-release"};
    std::string line;
    getline(osRelease, line);
    auto startPos{line.find('\"') + 1};
    auto endPos{line.rfind('\"')};
    return line.substr(startPos, endPos - startPos);
  }

  [[nodiscard]] auto getKernel() -> std::string {
    struct utsname name;
    uname(&name);
    return name.release;
  }

  [[nodiscard]] auto getProductName() -> std::string {
    std::ifstream product{"/sys/devices/virtual/dmi/id/product_family"};
    std::string line;
    getline(product, line);
    return line;
  }

  [[nodiscard]] auto getProductModel() -> std::string {
    std::ifstream product{"/sys/devices/virtual/dmi/id/product_name"};
    std::string line;
    getline(product, line);
    return line;
  }

  [[nodiscard]] auto getUptime() -> std::string {
    struct sysinfo info;
    sysinfo(&info);
    auto uptime{info.uptime};
    // auto days{uptime / 86400};
    auto hours{(uptime % 86400) / 3600};
    auto minutes{(uptime % 3600) / 60};
    // auto seconds{uptime % 60};
    if (hours == 0)
      return std::to_string(minutes) + "m";
    return std::to_string(hours) + "h " + std::to_string(minutes) + "m";
  }

  [[nodiscard]] auto getShell() -> std::string {
    std::string shell{getenv("SHELL")};
    return shell.substr(shell.rfind('/') + 1);
  }

  [[nodiscard]] auto getMemory() -> std::string {
    std::ifstream meminfo("/proc/meminfo");
    std::string line;
    getline(meminfo, line);
    auto start{line.find(':') + 1};
    auto end{line.rfind("kB")};
    auto total{stoi(line.substr(start, end - start)) >> 10};
    getline(meminfo, line);
    getline(meminfo, line);
    start = line.find(':') + 1;
    end = line.rfind("kB");
    auto available{stoi(line.substr(start, end - start)) >> 10};
    return std::to_string(total - available) + "M / " + std::to_string(total) +
           "M";
  }

  [[nodiscard]] auto getWM() -> std::string {
    std::string wm{getenv("DESKTOP_SESSION")};
    return wm.substr(wm.rfind('/') + 1);
  }

  [[nodiscard]] auto getTerminal() -> std::string {
    return std::string{getenv("TERM")};
  }

  [[nodiscard]] auto getEditor() -> std::string {
    std::string editor{getenv("EDITOR")};
    return editor.substr(editor.rfind('/') + 1);
  }

  [[nodiscard]] auto getPackages() -> std::string {
    std::unordered_map<std::string, std::string> packages;
    packages["pacman"] = "pacman -Q | wc -l";
    packages["apt"] = "dpkg -l | wc -l";
    packages["xbps"] = "xbps-query -l | wc -l";
    packages["apk"] = "apk info | wc -l";
    packages["eopkg"] = "eopkg list-installed | wc -l";
    packages["dnf"] = "dnf list installed | wc -l";
    packages["yum"] = "yum list installed | wc -l";
    packages["zypper"] = "zypper packages --installed-only | wc -l";
    packages["nix"] = "nix-env -q | wc -l";
    packages["guix"] = "guix package -l | wc -l";
    packages["pkg"] = "pkg info | wc -l";
    packages["pkg_add"] = "pkg_info | wc -l";
    packages["pkgin"] = "pkgin list | wc -l";
    packages["pkgng"] = "pkg info | wc -l";
    packages["tazpkg"] = "tazpkg list-installed | wc -l";
    packages["urpmi"] = "urpmq | wc -l";
    packages["urpmq"] = "urpmq | wc -l";
    packages["rpm"] = "rpm -qa | wc -l";
    packages["emerge"] = "qlist -I | wc -l";
    packages["equo"] = "equo query list installed | wc -l";
    packages["slapt-get"] = "slapt-get --installed | wc -l";
    packages["slackpkg"] = "slackpkg list | wc -l";
    packages["sorcery"] = "sorcery list | wc -l";
    uint32_t count{0};
    for (const auto &package : packages) {
      std::string command{"command -v " + package.first + " > /dev/null 2>&1"};
      if (system(command.c_str()) == 0) {
        system((package.second + " > /tmp/packages").c_str());
        std::ifstream packages{"/tmp/packages"};
        std::string line;
        getline(packages, line);
        count += stoi(line);
      }
    }
    return std::to_string(count);
  }

public:
  Fetch() {
    std::cout << "User: " << getUser() + "@" + getHost() << std::endl;
    std::cout << "OS: " << getOS() << std::endl;
    std::cout << "Kernel: " << getKernel() << std::endl;
    std::cout << "Product: " << getProductName() << std::endl;
    std::cout << "Model: " << getProductModel() << std::endl;
    std::cout << "Uptime: " << getUptime() << std::endl;
    std::cout << "Shell: " << getShell() << std::endl;
    std::cout << "Memory: " << getMemory() << std::endl;
    std::cout << "WM: " << getWM() << std::endl;
    std::cout << "Terminal: " << getTerminal() << std::endl;
    std::cout << "Editor: " << getEditor() << std::endl;
    std::cout << "Packages: " << getPackages() << std::endl;
  }
};

#endif