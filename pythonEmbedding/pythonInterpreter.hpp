#pragma once

#include <qobject.h>
#include <string>

#include <QString>

#pragma push_macro("slots")  // solve slots variable name conflits between python and qt.
#undef slots
#include <pybind11/embed.h>  // everything needed for embedding
#pragma pop_macro("slots")

// Reference: https://github.com/pybind/pybind11/issues/1622
class pyStdErrOutStreamRedirect
{
  public:
    pyStdErrOutStreamRedirect()
    {
        auto sysm = pybind11::module::import("sys");
        _stdout = sysm.attr("stdout");
        _stderr = sysm.attr("stderr");

        auto stringio = pybind11::module::import("io").attr("StringIO");
        _stdout_buffer =
            stringio();  // Other filelike object can be used here as well, such as objects created by pybind11
        _stderr_buffer = stringio();
        sysm.attr("stdout") = _stdout_buffer;
        sysm.attr("stderr") = _stderr_buffer;
    }

    ~pyStdErrOutStreamRedirect()
    {
        auto sysm = pybind11::module::import("sys");
        sysm.attr("stdout") = _stdout;
        sysm.attr("stderr") = _stderr;
    }

    std::string stdoutString()
    {
        _stdout_buffer.attr("seek")(0);
        return pybind11::str(_stdout_buffer.attr("read")());
    }
    std::string stderrString()
    {
        _stderr_buffer.attr("seek")(0);
        return pybind11::str(_stderr_buffer.attr("read")());
    }

  private:
    pybind11::object _stdout;
    pybind11::object _stderr;
    pybind11::object _stdout_buffer;
    pybind11::object _stderr_buffer;
};

class pythonInterpreter : public QObject
{
    Q_OBJECT

  public:
    [[nodiscard]] static std::vector<std::string> getPossibleMethods(const std::vector<std::string>& queryVec);

  public:
    explicit pythonInterpreter(QObject* parent = nullptr);

    void runCommand(const std::string& cmd) const;

  private:
    pybind11::scoped_interpreter interpreterGuard_;
    pybind11::dict locals_;

  signals:
    void pyCommandBeforeInserted(const QString& command) const;
    void pyCommandParsedWithError(const QString& message) const;
    void pyCommandStdOutput(const QString& message) const;
};
