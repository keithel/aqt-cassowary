// Copyright (c) 2014, 2015 Ableton AG, Berlin
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

#include <aqt/cassowary/ConstraintBase.hpp>

namespace aqt {
namespace cassowary {

ConstraintBase::ConstraintBase(QQuickItem* pParent,
                               Strength::Types strength,
                               double weight)
  : Contextual(pParent)
  , mStrength(strength)
  , mWeight(weight)
{
  connect(this, &ConstraintBase::strengthChanged, [this](Strength::Types) {
    if (!mConstraint.is_nil()) {
      auto ctx = context();
      auto installed = ctx && ctx->solver().contains_constraint(mConstraint);
      if (!installed) {
        mConstraint.change_strength(Strength::impl(mStrength));
      } else {
        log("Changing strength", mStrength, "->", mConstraint);
        ctx->solver().change_strength(mConstraint, Strength::impl(mStrength));
        ctx->defer([]{});
      }
    }
  });

  connect(this, &ConstraintBase::weightChanged, [this](double) {
    if (!mConstraint.is_nil()) {
      auto ctx = context();
      auto installed = ctx && ctx->solver().contains_constraint(mConstraint);
      if (!installed) {
        mConstraint.change_weight(mWeight);
      } else {
        log("Changing weight", mWeight, "->", mConstraint);
        ctx->solver().change_weight(mConstraint, mWeight);
        ctx->defer([]{});
      }
    }
  });

  connect(this, &ConstraintBase::whenChanged, [this](bool when) {
    update ([this, when] {
      mActualWhen = when;
    });
  });
}

void ConstraintBase::set(std::shared_ptr<rhea::abstract_constraint> constraint)
{
  update([this, constraint]() mutable {
    mConstraint = std::move(constraint);
    if (!mConstraint.is_nil()) {
      mConstraint.change_weight(mWeight);
      mConstraint.change_strength(Strength::impl(mStrength));
    }
  });
}

void ConstraintBase::addIn(Context& ctx)
{
  if (when() && !mConstraint.is_nil()) {
    auto constraint = mConstraint;
    auto id = constraint.hash();
    ctx.add(id, guarded([constraint, this, &ctx] {
      log("Add:", constraint);
      try {
        ctx.solver().add_constraint(constraint);
      } catch (const rhea::required_failure&) {
        log("Required constraint can't be satisfied:", constraint);
      }
    }));
  }
}

void ConstraintBase::removeIn(Context& ctx)
{
  if (when() && !mConstraint.is_nil()) {
    auto constraint = mConstraint;
    auto id = constraint.hash();
    ctx.remove(id, guarded([constraint, this, &ctx] {
      try {
        log("Remove:", mConstraint);
        ctx.solver().remove_constraint(constraint);
      } catch (const rhea::constraint_not_found&) {
        log("Constraint not found:", constraint);
      }
    }));
  }
}

} // namespace cassowary
} // namespace aqt
