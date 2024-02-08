#pragma once

#include <qobject.h>
#include <string>

#include <QString>

#pragma push_macro("slots")  // solve slots variable name conflits between python and qt.
#undef slots
#include <pybind11/embed.h>  // everything needed for embedding
#pragma pop_macro("slots")

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
    void commandInserted(const QString& command) const;
    void commandParsedWithError(const QString& message) const;
};
