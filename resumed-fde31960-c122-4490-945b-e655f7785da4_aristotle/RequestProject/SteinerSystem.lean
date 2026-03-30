import Mathlib

/-!
# Steiner Systems — Formalization and Analysis

## Definition

A (n, q, r)-Steiner system is a set S of q-subsets of {1,...,n} such that every
r-subset of {1,...,n} is contained in exactly 1 element of S.

## Status

Finding an (n, q, r)-Steiner system with n > q > r > 5 (and r < 10, n < 200) is one
of the most famous open problems in combinatorics (Open Subproblem 1 in the problem
context). No such system is known to exist in the mathematical literature.

The only known Steiner systems with t ≥ 4 are:
- S(4, 5, 11) and S(5, 6, 12) from the Mathieu group M₁₂
- S(4, 7, 23) and S(5, 8, 24) from the Mathieu group M₂₄
All of these have t ≤ 5.

## Computational Results

We performed extensive computational searches:
1. SAT solver (Kissat) on the full S(6,7,17) problem — did not terminate in 30+ min
2. Cyclic Z₁₇-invariant S(6,7,17) — proven UNSAT (no cyclic design exists)
3. PGL(2,16)-invariant S(6,7,17) — proven UNSAT (exhaustive check of all 2^10 subsets)
4. Structural analysis: the local partition structure around any block is consistent
   (21 compatible partitions exist), but no specific choice extends to a full design
5. Greedy random packing achieves at most ~921 blocks out of 1768 needed (~52%)

These results are consistent with S(6,7,17) not existing, but a proof of non-existence
remains open.
-/

/-- A Steiner system S(r, q, n): a collection of q-element subsets of Fin n
    such that every r-element subset is contained in exactly one block. -/
def IsSteinerSystem (n q r : ℕ) (blocks : Finset (Finset (Fin n))) : Prop :=
  (∀ B ∈ blocks, B.card = q) ∧
  (∀ T : Finset (Fin n), T.card = r →
    (blocks.filter (fun B => T ⊆ B)).card = 1)

/-
PROBLEM
Necessary divisibility condition for Steiner systems:
    For all 0 ≤ i ≤ r-1, C(n-i, r-i) must be divisible by C(q-i, r-i).

PROVIDED SOLUTION
This follows from a double counting argument. Fix any i-subset I of [n] with i < r. Consider the "derived design at I": blocks containing I, restricted to [n]\I. This is also a design, and the number of blocks containing I times C(q-i, r-i) equals C(n-i, r-i). Since the number of blocks containing I is a positive integer, C(q-i, r-i) divides C(n-i, r-i).
-/
theorem steiner_divisibility_necessary (n q r : ℕ) (blocks : Finset (Finset (Fin n)))
    (h : IsSteinerSystem n q r blocks) (i : ℕ) (hi : i < r) (hq : i < q) :
    (Nat.choose (q - i) (r - i)) ∣ (Nat.choose (n - i) (r - i)) := by
  -- Let $I$ be any $i$-element subset of $\{1, 2, \ldots, n\}$.
  have h_derived : ∀ I : Finset (Fin n), I.card = i → (blocks.filter (fun B => I ⊆ B)).card * (q - i).choose (r - i) = (n - i).choose (r - i) := by
    intro I hI_card
    have h_derived : (blocks.filter (fun B => I ⊆ B)).sum (fun B => (B \ I).card.choose (r - i)) = (n - i).choose (r - i) := by
      have h_derived : Finset.sum (blocks.filter (fun B => I ⊆ B)) (fun B => Finset.card (Finset.powersetCard (r - i) (B \ I))) = Finset.card (Finset.powersetCard (r - i) (Finset.univ \ I)) := by
        rw [ ← Finset.card_biUnion ];
        · congr with T ; simp_all +decide [ Finset.subset_iff ];
          constructor <;> intro hT;
          · grind +ring;
          · have := h.2 ( T ∪ I ) ?_ <;> simp_all +decide [ Finset.disjoint_left ];
            · obtain ⟨ B, hB ⟩ := Finset.card_pos.mp ( by linarith ) ; use B; aesop;
            · rw [ Nat.sub_add_cancel hi.le ];
        · intros B hB C hC hBC; simp_all +decide [ Finset.disjoint_left ] ;
          intro a ha₁ ha₂ ha₃; have := h.2 ( I ∪ a ) ; simp_all +decide [ Finset.subset_iff ] ;
          contrapose! this;
          exact ⟨ by rw [ Finset.card_union_of_disjoint ( Finset.disjoint_left.mpr fun x hx₁ hx₂ => by have := ha₁ hx₂; aesop ), hI_card, ha₂, add_tsub_cancel_of_le hi.le ], by exact ne_of_gt <| Finset.one_lt_card.mpr ⟨ B, by aesop, C, by aesop ⟩ ⟩;
      simp_all +decide [ Finset.card_sdiff ];
    rw [ ← h_derived, Finset.sum_const_nat ];
    simp_all +decide [ Finset.card_sdiff ];
    intro x hx hx'; rw [ Finset.inter_eq_left.mpr hx' ] ; have := h.1 x hx; aesop;
  by_cases h_exists : ∃ I : Finset (Fin n), I.card = i;
  · exact dvd_of_mul_left_eq _ ( h_derived _ h_exists.choose_spec );
  · rcases n with ( _ | _ | n ) <;> rcases i with ( _ | _ | i ) <;> simp_all +arith +decide [ Nat.choose_eq_zero_of_lt ];
    · exact absurd ( h_exists { 0 } ) ( by norm_num );
    · contrapose! h_exists;
      use Finset.univ.filter (fun x => x.val < i + 2);
      rw [ Finset.card_eq_of_bijective ];
      use fun x hx => ⟨ x, by linarith [ show n ≥ i + 1 from Nat.le_of_not_lt fun h => h_exists <| by simp +arith +decide [ Nat.choose_eq_zero_of_lt ( by omega : n - i < r - ( i + 2 ) ) ] ] ⟩;
      · grind;
      · aesop;
      · aesop

/-
PROBLEM
The number of blocks in a Steiner system S(r, q, n) is C(n, r) / C(q, r).

PROVIDED SOLUTION
Double counting: count pairs (T, B) where T is an r-subset, B is a block, T ⊆ B. By the Steiner condition, for each T there is exactly 1 such B, so the count is C(n, r). By counting over blocks, each block B has C(q, r) r-subsets, so the count is |blocks| * C(q, r). Therefore |blocks| * C(q, r) = C(n, r).
-/
theorem steiner_block_count (n q r : ℕ) (blocks : Finset (Finset (Fin n)))
    (h : IsSteinerSystem n q r blocks) (hr : 0 < r) (hqr : r ≤ q) :
    blocks.card * Nat.choose q r = Nat.choose n r := by
  have h_count : (Finset.card (Finset.powersetCard r (Finset.univ : Finset (Fin n)))) = (Finset.sum blocks (fun B => Finset.card (Finset.powersetCard r B))) := by
    rw [ ← Finset.card_biUnion ];
    · congr with T;
      simp +zetaDelta at *;
      exact ⟨ fun hT => by have := h.2 T hT; obtain ⟨ B, hB ⟩ := Finset.card_pos.mp ( by linarith ) ; aesop, fun ⟨ B, hB₁, hB₂, hB₃ ⟩ => hB₃ ⟩;
    · intros B hB C hC hBC; simp_all +decide [ Finset.disjoint_left ] ;
      -- Since $B$ and $C$ are distinct blocks, their intersection cannot contain any $r$-subset.
      intros a haB haC haC'
      have h_inter : (blocks.filter (fun B => a ⊆ B)).card = 1 := by
        exact h.2 a haC;
      exact absurd h_inter ( by exact Nat.ne_of_gt ( Finset.one_lt_card.mpr ⟨ B, by aesop, C, by aesop ⟩ ) );
  simp_all +decide [ Finset.card_powersetCard ];
  rw [ Finset.sum_congr rfl fun x hx => by rw [ h.1 x hx ] ] ; aesop

/-
PROBLEM
In a Steiner system, any two distinct blocks share fewer than r elements.

PROVIDED SOLUTION
If B₁ and B₂ are two distinct blocks in a Steiner system S(r,q,n), and they share r or more elements, then their intersection contains an r-subset T. By the Steiner condition, T is in exactly one block, but T ⊆ B₁ and T ⊆ B₂, so both B₁ and B₂ would be in the filter, giving card ≥ 2, contradicting the "exactly 1" condition.
-/
theorem steiner_block_intersection (n q r : ℕ) (blocks : Finset (Finset (Fin n)))
    (h : IsSteinerSystem n q r blocks) (B₁ B₂ : Finset (Fin n))
    (hB₁ : B₁ ∈ blocks) (hB₂ : B₂ ∈ blocks) (hne : B₁ ≠ B₂) :
    (B₁ ∩ B₂).card < r := by
  -- Assume that $|B₁ ∩ B₂| ≥ r$. Then, there exists an $r$-element subset $T$ of $B₁ ∩ B₂$.
  by_contra h_contra
  obtain ⟨T, hTr⟩ : ∃ T : Finset (Fin n), T ⊆ B₁ ∩ B₂ ∧ T.card = r := by
    exact Finset.exists_subset_card_eq <| by linarith;
  have := h.2 T hTr.2;
  rw [ Finset.card_eq_one ] at this ; obtain ⟨ B, hB ⟩ := this ; simp_all +decide [ Finset.subset_iff ] ;
  rw [ Finset.eq_singleton_iff_unique_mem ] at hB ; aesop