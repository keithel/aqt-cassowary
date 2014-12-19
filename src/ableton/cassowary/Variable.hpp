// Copyright: 2014, Ableton AG, Berlin, all rights reserved

#pragma once

#include <ableton/cassowary/Contextual.hpp>
#include <ableton/build_system/Warnings.hpp>

ABL_DISABLE_WARNINGS
#include <rhea/variable.hpp>
ABL_RESTORE_WARNINGS

namespace ableton {
namespace cassowary {

class Variable : public Contextual
{
  Q_OBJECT

public:
  Variable(QQuickItem* pParent = 0);

  Q_PROPERTY(double value READ value NOTIFY valueChanged)
  double value() const;
  Q_SIGNAL void valueChanged(double value);

  Q_PROPERTY(double initial MEMBER mInitial NOTIFY initialChanged)
  Q_SIGNAL void initialChanged(double initial);

  const rhea::variable& variableImpl() const;

protected:
  void addIn(Context&) override {}
  void removeIn(Context&) override {}

private:
  rhea::variable mVariable;
  double mInitial = 0.0;
};

} // namespace cassowary
} // namespace ableton
